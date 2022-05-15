#include "MQTTListener2.h"
#include <iostream>
#include <raylib.h>

using namespace std;


MQTTListener2::MQTTListener2(MQTTClient2* mqtt)
{
	miau = mqtt;
	ballPos.resize(3);
	ballVel.resize(3);
	ballRot.resize(3);
	ballAngVel.resize(3);
}

void MQTTListener2::onMessage(string topic, vector<char> payload)
{
	bool change = false;
	if (topic == "ball/motion/state")
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
		printVector(ballPos);
		printVector(ballVel);
		printVector(ballRot);
		printVector(ballAngVel);
		lastPayload = payload;
		change = true;
		}
	}
	ballPos[1] = ballPos[2];
	ballPos[2] = 45;
	if (change)
	{
		vector<char> mensaje(12);
		for (int i = 0; i < ballPos.size(); i++)
		{
			memcpy(&(mensaje[i * sizeof(float)]), &(ballPos[i]), sizeof(float));
		}
		miau->publish("robot1.1/pid/setpoint/set", mensaje);
		cout << "cambio" << endl;
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
