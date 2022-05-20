/**
 * @file GameController.cpp
 * @author Grupo 6: Mariano Ohms, Segundo Tanoira, Lucia Ruiz, Valentin Vieira
 * @brief Controls the Game
 * @date 2022-05-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "GameController.h"

using namespace std;

#define HEATMAPROWS 60
#define HEATMAPCOLUMNS 90

/**
 * @brief Construct a new GameController::GameController object and intialize the data members
 * @param mqtt the player to be controlled
 */
GameController::GameController(MQTTClient2* mqtt)
{
	MQTTClient = mqtt;

	change = false;
	timer = 0;

	ballPos.resize(3);
	ballVel.resize(3);
	playerRot.resize(3);
	playerPos.resize(3);

	image = LoadImage("../../../Images/1EDACUP.png");


	/*
	void Robot::setDisplay(int imageIndex)
{
    Rectangle selectRectangle = {16.0F * imageIndex, 0, 16, 16};
    Image selectedImage = ImageFromImage(displayImages, selectRectangle);

    const int dataSize = 16 * 16 * 3;
    vector<char> payload(dataSize);
    memcpy(payload.data(), selectedImage.data, dataSize);

    UnloadImage(selectedImage);

    mqttClient->publish(robotId + "/display/lcd/set", payload);
}*/


	ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);

	Rectangle selectRectangle = { 16.0F * 0, 0, 16, 16 };
	Image selectedImage = ImageFromImage(image, selectRectangle);

	const int dataSize = 16 * 16 * 3;
    vector<char> payload(dataSize);
    memcpy(payload.data(), selectedImage.data, dataSize);

	UnloadImage(selectedImage);

	MQTTClient->publish("robot1.1/display/lcd/set", payload);
}

/**
 * @brief Destroy the Game Controller:: Game Controller object
 * 
 */
GameController::~GameController()
{
	UnloadImage(image);
}

/**
 * @brief with the information received, control the game
 * @param topic strings that constains a path of diferent elements 
 * @param payload containt data of a topic given
 */
void GameController::onMessage(string topic, vector<char> payload)
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

			if ((abs(goalAngle - ballAngle) < 10) && kick)		// this line calculate the error to kick
			{
				MQTTClient->publish("robot1.1/kicker/kick/cmd", message);
				kick = false;
			}

			break;
		}
		change = false;
	}

	if (timer <= 0)
	{
		change = true;
	}
	if (timer > 0)
		timer--;

}

/**
 * @brief Calculates angle from the points given
 * @param start First point
 * @param finish Second point
 * @return Angle
 */
float GameController::angleCalculator(vector<float> start, vector<float> finish)
{
	float leg1 = finish[0] - start[0];
	float leg2 = finish[2] - start[2];
	float angle = atan2(leg1, leg2) * 180 / PI;
	if (angle < 0)
		angle += 360;
	return angle;
}

/**
 * @brief Moves the robot to a position
 * @param setPoint the point where the robot will go
 */
void GameController::moveRobotToSetPoint(vector<float> setPoint)
{
	vector<char> mensaje(12);
	memcpy(mensaje.data(), setPoint.data(), 3 * sizeof(float));
	MQTTClient->publish("robot1.1/pid/setpoint/set", mensaje);
}

/**
 * @brief given the end position, divide the path into intervals and return the next position
 * @param destinationPoint 
 * @return next position
 */
vector<float> GameController::getSetPoint(vector<float> destinationPoint)
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
 * @brief given the final angle, divide the path into intervals and return the next angle
 * @param destination 
 * @return next angle
 */
float GameController::getSetAngle(vector<float> destination)
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