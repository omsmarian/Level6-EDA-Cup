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

#define MIN_DISTANCE 0.01
#define CHARGE_VALUE 200
#define MINIMAL_ERROR 10
#define DELTA_ANGLE 28
#define DELTA_DISTANCE 1

using namespace std;

/**
 * @brief Construct a new GameController::GameController object, intialize the data members
 * and set the display
 * @param mqtt the player to be controlled
 */
GameController::GameController(MQTTClient2* mqtt)
{
	MQTTClient = mqtt;

	kick = true;
	update = false;
	timer = 0;

	ballPos.resize(3);
	playerRot.resize(3);
	playerPos.resize(3);

	image = LoadImage("../../../Images/image.png");
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
			for (int i = 0; i < 3; i++)
			{
				memcpy(&(ballPos[i]), &(payload[i * sizeof(float)]), sizeof(float));
			}
			lastPayload = payload;
		}
	}
	vector<char> message(4);
	float voltage;

	vector<float> setPoint;
	vector<float> goal = { -4.5,0,0 };

	float ballAngle;
	float goalAngle;

	if (ballPos[1] > 0.1)
		kick = true;

	if (kick)
	{
		if (((ballPos[0] - playerPos[0]) * (ballPos[0] - playerPos[0]) +
			(ballPos[2] - playerPos[2]) * (ballPos[2] - playerPos[2])) >= MIN_DISTANCE)
		{
			playerState = goingToBall;
		}
		else
			playerState = atBall;
	}

	if (update)
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

			voltage = CHARGE_VALUE;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish("robot1.1/kicker/chargeVoltage/set", message);

			break;

		case atBall:

			timer = 1;
			voltage = 3;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish("robot1.1/dribbler/voltage/set", message);

			setPoint = { playerPos[0], playerPos[2], getSetAngle(goal) };

			moveRobotToSetPoint(setPoint);

			ballAngle = angleCalculator(playerPos, ballPos);

			goalAngle = angleCalculator(playerPos, goal);

			if ((abs(goalAngle - ballAngle) < MINIMAL_ERROR) && kick)		// calculates the aceptable error to kick
			{
				MQTTClient->publish("robot1.1/kicker/kick/cmd", message);
				kick = false;
			}

			break;
		}
		update = false;
	}

	if (timer <= 0)
	{
		update = true;
	}
	if (timer > 0)
		timer--;

}

/**
 * @brief Calculates angle from the points given
 * @param start, first point
 * @param finish, second point
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
 * @param setPoint, the point where the robot will go
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

	if (distance > DELTA_DISTANCE)
		destination = Vector2Add(currentPosition, Vector2Scale(deltaVector, DELTA_DISTANCE / distance));

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
	if (abs(substraction) > DELTA_ANGLE)
	{
		if (abs(substraction) < 180)
			return (initialAngle - (DELTA_ANGLE * ((substraction > 0) - (substraction < 0))));
		else
			return (initialAngle + (DELTA_ANGLE * ((substraction > 0) - (substraction < 0))));
	}
	else
		return targetAngle;
}