#include "MQTTListener2.h"

using namespace std;

#define HEATMAPROWS 60
#define HEATMAPCOLUMNS 90

MQTTListener2::MQTTListener2(MQTTClient2* mqtt)
{
	MQTTClient = mqtt;
	ballPos.resize(3);
	ballVel.resize(3);
	ballRot.resize(3);
	ballAngVel.resize(3);
	playerPos.resize(3);
}

void MQTTListener2::onMessage(string topic, vector<char> payload)
{
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
		}
	}

	vector<char> mensajeVoltage(4);
	float voltage;

	if (ballPos[1] > 0.1)
		kick = true;

	if (kick)
	{
		if (((ballPos[0] - playerPos[0]) * (ballPos[0] - playerPos[0]) + (ballPos[2] - playerPos[2]) * (ballPos[2] - playerPos[2])) >= 0.01)
		{
			playerState = goingToBall;
			voltage = 0;
			memcpy(&(mensajeVoltage[0]), &voltage, sizeof(float));
			MQTTClient->publish("robot1.1/dribbler/voltage/set", mensajeVoltage);
		}
		else
		{
			playerState = atBall;
		}
	}

	vector<float> setPoint;
	vector<float> goal = { 4.5,0,0 };
	vector<float> palo1 = { 4.5,0,0.5 };
	vector<float> palo2 = { 4.5,0,-0.5 };
	vector<float> playerPosBehind;

	int anguloPelota;
	int anguloArco;

	switch (playerState)
	{
	case goingToBall:

		setPoint = getSetPoint(ballPos);
		moveRobotToSetPoint(setPoint);
		voltage = 200;
		memcpy(&(mensajeVoltage[0]), &voltage, sizeof(float));
		MQTTClient->publish("robot1.1/kicker/chargeVoltage/set", mensajeVoltage);
		break;

	case atBall:

		setPoint = { playerPos[0], playerPos[2], angleCalculator(playerPos, goal) };

		voltage = 1;
		memcpy(&(mensajeVoltage[0]), &voltage, sizeof(float));
		MQTTClient->publish("robot1.1/dribbler/voltage/set", mensajeVoltage);
		moveRobotToSetPoint(setPoint);
		
		anguloPelota = (int)angleCalculator(playerPos, ballPos);
		
		anguloArco = (int)angleCalculator(playerPos, goal);

		playerPosBehind = {playerPos[0]-0.7f*sinf(anguloArco*PI/180), playerPos[1], 
							playerPos[2]-0.7f*cosf(anguloArco*PI/180)};


		if((anguloPelota >= (int)angleCalculator(playerPosBehind, palo1) && 
			anguloPelota < (int)angleCalculator(playerPosBehind, palo2)) && 
			( (playerPos[1] - anguloArco) < 1 ) && kick)
		{
			cout << "pateo" << endl;
			MQTTClient->publish("robot1.1/kicker/kick/cmd", mensajeVoltage);
			kick = false;
		}
		break;
	}
}

void MQTTListener2::printVector(std::vector<float> vector)
{
	cout << "vector: ";
	for (auto x : vector)
		cout << x << ", ";
	cout << endl;
}

float MQTTListener2::angleCalculator(vector<float> start, vector<float> finish)
{
	float cateto1 = finish[0] - start[0];
	float cateto2 = finish[2] - start[2];
	float angle = atan2(cateto1, cateto2) * 180 / PI;
	if (angle < 0)
		angle += 360;
	return angle;
}


void MQTTListener2::moveRobotToSetPoint(vector<float> setPoint)
{
	int tiempo = 10;
	vector<char> mensaje(12);
	for (int i = 0; i < setPoint.size(); i++)
	{
		memcpy(&(mensaje[i * sizeof(float)]), &(setPoint[i]), sizeof(float));
	}
	MQTTClient->publish("robot1.1/pid/setpoint/set", mensaje);
	while (tiempo)
		tiempo--;
}

vector<float> MQTTListener2::getSetPoint(vector<float> destinationPoint)
{
	Vector2 deltaVector = { destinationPoint[0] - playerPos[0], destinationPoint[2] - playerPos[2] };
	Vector2 currentPosition = { playerPos[0], playerPos[2] };
	Vector2 destination = { destinationPoint[0] , destinationPoint[2] };
	float distance = Vector2Distance(currentPosition, destination);
	float angle;

	if (distance > 1)
		destination = Vector2Add(currentPosition, Vector2Scale(deltaVector, 1 / distance));

	vector<float> setPoint = { destination.x, destination.y, angleCalculator(playerPos, destinationPoint)};
	return setPoint;
}

/*
 pair<int, int> actualPositionToHeatMapPosition(vector<float> actualPosition)
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
}*/
