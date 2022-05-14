#include "MQTTListener2.h"

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
	if (topic == "ball/motion/staet")
	{
		for (int i = 0; i < 12; i++)
		{
			if ((i / 3) == 0)
				memcpy(&(ballPos[i]), &(payload[i * sizeof(float)]), sizeof(float));
			else if ((i / 3) == 1)
				memcpy(&(ballVel[i]), &(payload[i * sizeof(float)]), sizeof(float));
			else if ((i / 3) == 2)
				memcpy(&(ballRot[i]), &(payload[i * sizeof(float)]), sizeof(float));
			else if ((i / 3) == 3)
				memcpy(&(ballAngVel[i]), &(payload[i * sizeof(float)]), sizeof(float));
		}
	}
}
