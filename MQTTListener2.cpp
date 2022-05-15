#include "MQTTListener2.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <utility>

using namespace std;

#define HEATMAPROWS 60
#define HEATMAPCOLUMNS 90

MQTTListener2::MQTTListener2(MQTTClient2* mqtt)
{
	miau = mqtt;
	ballPos.resize(3);
	ballVel.resize(3);
	ballRot.resize(3);
	ballAngVel.resize(3);
	playerPos.resize(3);
}

void MQTTListener2::onMessage(string topic, vector<char> payload)
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
			for (int i = 0, j = 0; i < 12; i++)
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
	ballPos[1] = ballPos[2];
	ballPos[2] = angleCalculator(playerPos, ballPos);
	if (change)
	{
		vector<char> mensaje(12);
		for (int i = 0; i < ballPos.size(); i++)
		{
			memcpy(&(mensaje[i * sizeof(float)]), &(ballPos[i]), sizeof(float));
		}
		miau->publish("robot1.1/pid/setpoint/set", mensaje);
		change = false;
	}

}

void MQTTListener2::printVector(std::vector<float> vector)
{
	cout << "vector: ";
	for (auto x : vector)
		cout << x << ", ";
	cout << endl;
}

float MQTTListener2::angleCalculator(vector<float> start,vector<float> finish)
{
	float cateto1 = finish[0] - start[0];
	float cateto2 = finish[2] - start[2];
	float angle = atanf(cateto1 / cateto2) * 180 / PI;

	return angle;
}

 
 
