#include "MQTTListener2.h"

using namespace std;

#define HEATMAPROWS 60
#define HEATMAPCOLUMNS 90

MQTTListener2::MQTTListener2(MQTTClient2* mqtt)
{
	MQTTClient = mqtt;

	change = false;
	timer = 0;

	ballPos.resize(3);
	ballVel.resize(3);
	playerRot.resize(3);
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
			for (int i = 0; i < 6; i++)
			{
				if ((i / 3) == 0)
					memcpy(&(ballPos[i]), &(payload[i * sizeof(float)]), sizeof(float));
				else if ((i / 3) == 1)
					memcpy(&(ballVel[i - 3]), &(payload[i * sizeof(float)]), sizeof(float));
			}
			lastPayload = payload;
		}
	}
	vector<char> message(4);
	float voltage;

	vector<float> setPoint;
	vector<float> goal = { 4.5,0,0 };

	float ballAngle;
	float goalAngle;

	if (ballPos[1] > 0.1)
		kick = true;

	if (kick)
	{
		if (((ballPos[0] - playerPos[0]) * (ballPos[0] - playerPos[0]) +
			(ballPos[2] - playerPos[2]) * (ballPos[2] - playerPos[2])) >= 0.01)
		{
			playerState = goingToBall;
		}
		else
			playerState = atBall;
	}

	if (change)
	{

		switch (playerState)
		{
		case goingToBall:
			timer = 3;

			voltage = 0;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish("robot1.1/dribbler/voltage/set", message);

			setPoint = getSetPoint(ballPos);
			moveRobotToSetPoint(setPoint);

			voltage = 200;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish("robot1.1/kicker/chargeVoltage/set", message);

			break;

		case atBall:

			timer = 1;
			voltage = 3;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish("robot1.1/dribbler/voltage/set", message);		//esto tmb puede ser una funcion; ej: void dribbleState(float x)

			setPoint = { playerPos[0], playerPos[2], getSetAngle(goal) };

			moveRobotToSetPoint(setPoint);

			ballAngle = angleCalculator(playerPos, ballPos);

			goalAngle = angleCalculator(playerPos, goal);

			if ((abs(goalAngle - ballAngle) < 10) && kick)		// esto calcula q el error sea tolerable, entonces patea
			{
				MQTTClient->publish("robot1.1/kicker/kick/cmd", message);			//todo lo del if puede ser una funcion
				kick = false;
			}

			break;
		}
		change = false;
	}

	if (timer <= 0)
	{
		change = true;				//esta cadena de ifs puede ser una funcion; ej void delay(int timer)
	}
	if (timer > 0)
		timer--;

}

void MQTTListener2::printVector(std::vector<float> vector)
{
	cout << "vector: ";
	for (auto x : vector)
		cout << x << ", ";
	cout << endl;
}
/**
 * @brief Calculates angle from the points given
 * @param First point
 * @param Second point
 * @return Angle
 */
float MQTTListener2::angleCalculator(vector<float> start, vector<float> finish)
{
	float cateto1 = finish[0] - start[0];
	float cateto2 = finish[2] - start[2];
	float angle = atan2(cateto1, cateto2) * 180 / PI;
	if (angle < 0)
		angle += 360;
	return angle;
}

/**
 * @brief Moves the robot
 */
void MQTTListener2::moveRobotToSetPoint(vector<float> setPoint)
{
	vector<char> mensaje(12);
	memcpy(mensaje.data(), setPoint.data(), 3 * sizeof(float));
	MQTTClient->publish("robot1.1/pid/setpoint/set", mensaje);
}

/**
 * @brief Calculate the next position
 */
vector<float> MQTTListener2::getSetPoint(vector<float> destinationPoint)
{
	Vector2 deltaVector = { destinationPoint[0] - playerPos[0],
		destinationPoint[2] - playerPos[2] };
	Vector2 currentPosition = { playerPos[0], playerPos[2] };
	Vector2 destination = { destinationPoint[0] , destinationPoint[2] };
	float distance = Vector2Distance(currentPosition, destination);

	if (distance > 1)
		destination = Vector2Add(currentPosition, Vector2Scale(deltaVector, 1 / distance));

	vector<float> setPoint = { destination.x, destination.y,
		angleCalculator(playerPos, destinationPoint) };
	return setPoint;
}

/**
 * @brief Calculate the next angle (en intervalos)
 */
float MQTTListener2::getSetAngle(vector<float> destination)
{
	float initialAngle = angleCalculator(playerPos, ballPos);
	float targetAngle = angleCalculator(playerPos, destination);

	float substraction = initialAngle - targetAngle;
	if (abs(substraction) > 28)
	{
		if (abs(substraction) < 180)
			return (initialAngle - (28 * ((substraction > 0) - (substraction < 0))));
		else
			return (initialAngle + (28 * ((substraction > 0) - (substraction < 0))));
	}
	else
		return targetAngle;
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
