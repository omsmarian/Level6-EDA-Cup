#include "GameController.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <raymath.h>
#include <utility>

using namespace std;

// #define HEATMAPROWS 60
// #define HEATMAPCOLUMNS 90

GameController::GameController(MQTTClient2* mqtt)
{
	robot = mqtt;
	ballPos.resize(3);
	ballVel.resize(3);
	ballRot.resize(3);
	ballAngVel.resize(3);
	playerPos.resize(3);
}

void GameController::onMessage(string topic, vector<char> payload)
{
	bool change = false;
	if (topic == "robot1.1/motion/state")
	{
		for (int j = 0; j < 3; j++)
		{
			memcpy(&(playerPos[j]), &(payload[j * sizeof(float)]), sizeof(float));
		}
	}
	else if (topic == "ball/motion/state")
	{
		if (lastPayload != payload)
		{
			for (int i = 0; i < 12; i++)
			{
				if ((i / 3) == 0)
					memcpy(&(ballPos[i]), &(payload[i * sizeof(float)]), sizeof(float));
				else if ((i / 3) == 1)
					memcpy(&(ballVel[i - 3]), &(payload[i * sizeof(float)]), sizeof(float));
				else if ((i / 3) == 2)
					memcpy(&(ballRot[i - 6]), &(payload[i * sizeof(float)]), sizeof(float));
				else if ((i / 3) == 3)
					memcpy(&(ballAngVel[i - 9]), &(payload[i * sizeof(float)]), sizeof(float));
			}
			lastPayload = payload;
			change = true;
		}
	}
	printVector(ballPos);
	printVector(playerPos);

	Vector2 vec2PlayerPos = {playerPos[0], playerPos[2]};
	Vector2 vec2BallPos = {ballPos[0], ballPos[1]};
	cout << Vector2Distance(vec2BallPos, vec2PlayerPos) << endl;
	if(Vector2Distance(vec2BallPos, vec2PlayerPos) < 1)
	{
		// cout << "frena conche su madre" << endl;
	}
	
		setRobotDestinationPoint(change, ballPos);

}

void GameController::printVector(std::vector<float> vector)
{
	cout << "vector: ";
	for (auto x : vector)
		cout << x << ", ";
	cout << endl;
}

float GameController::angleCalculator(vector<float> start, vector<float> finish)
{
	float cateto1 = finish[0] - start[0];
	float cateto2 = finish[2] - start[2];
	float angle = atanf(cateto1 / cateto2) * 180 / PI;
	if(angle < 0)
	{
		angle = 360 - angle;
	}
	return angle;
}

void GameController::setRobotDestinationPoint(bool change, vector<float> &destination)
{
	if (change)
	{
		vector<char> mensaje(12);
		for (int i = 0; i < ballPos.size(); i++)
		{
			memcpy(&(mensaje[i * sizeof(float)]), &(destination[i]), sizeof(float));
		}
		robot->publish("robot1.1/pid/setpoint/set", mensaje);
		change = false;
	}
}

pair<int, int> GameController::actualPositionToHeatMapPosition(vector<float> actualPosition)
{
    vector<int> auxActualPosition(2);


    auxActualPosition[0] = actualPosition[0]*10 + 45;
    auxActualPosition[1] = -(actualPosition[1]*10 - 30);


    if(auxActualPosition[0] == 90)
    {
        auxActualPosition[0] -= 1;
    }
    if(auxActualPosition[1] == 60)
    {
        auxActualPosition[1] -= 1;
    }

    pair<int, int> index;
    index.first = auxActualPosition[0];
    index.second = auxActualPosition[1];

    return index;
}

void GameController::makeHeatMap(int** heatMap)
{
	for(int i=0; i < ROW; i++)
	{
		for(int j=0; j < COL; j++)
		{
			heatMap[i][j]  = 0;
		}
	}
}

