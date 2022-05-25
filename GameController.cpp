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
GameController::GameController(MQTTClient2* mqtt, vector<Player*> auxList)
{
	MQTTClient = mqtt;
	for(int i = 5 ; i >= 0 ; i--)
	{
		playerList.push_back(auxList[i]);
	}

	kick = true;
	update = false;
	timer = 0;
}

/**
 * @brief Destroy the Game Controller:: Game Controller object
 *
 */
GameController::~GameController()
{

}


/**
 * @brief with the information received, control the game
 * @param topic strings that constains a path of diferent elements
 * @param payload containt data of a topic given
 */
void GameController::onMessage(string topic, vector<char> payload)
{
	
	if (topic == (playerList[0]->robotId + "/motion/state"))
	{
		for (int j = 0; j < 3; j++)
		{
			memcpy(&(playerList[0]->playerPos[j]), &(payload[j * sizeof(float)]), sizeof(float));
		}
	}
	else if (topic == "ball/motion/state")
	{
		if (lastPayload != payload)
		{
			for (int i = 0; i < 3; i++)
			{
				float aux = playerList[0]->ballPos[i];
				memcpy(&(playerList[0]->ballPos[i]) , &(payload[i * sizeof(float)]), sizeof(float));
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

	if (playerList[0]->ballPos[1] > 0.1)
		kick = true;

	if (kick)
	{
		if (((playerList[0]->ballPos[0] - playerList[0]->playerPos[0]) * (playerList[0]->ballPos[0] - playerList[0]->playerPos[0]) +
			(playerList[0]->ballPos[2] - playerList[0]->playerPos[2]) * (playerList[0]->ballPos[2] - playerList[0]->playerPos[2])) >= MIN_DISTANCE)
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
			MQTTClient->publish(playerList[0]->robotId + "/dribbler/voltage/set", message);

			setPoint = playerList[0]->getSetPoint(playerList[0]->ballPos);
			playerList[0]->moveRobotToSetPoint(setPoint);

			voltage = CHARGE_VALUE;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish(playerList[0]->robotId + "/kicker/chargeVoltage/set", message);

			break;

		case atBall:

			timer = 1;
			voltage = 3;
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish(playerList[0]->robotId + "/dribbler/voltage/set", message);

			setPoint = { playerList[0]->playerPos[0], playerList[0]->playerPos[2], playerList[0]->getSetAngle(goal) };


			playerList[0]->moveRobotToSetPoint(setPoint);

			ballAngle = playerList[0]->angleCalculator(playerList[0]->playerPos, playerList[0]->ballPos);

			goalAngle = playerList[0]->angleCalculator(playerList[0]->playerPos, goal);

			if ((abs(goalAngle - ballAngle) < MINIMAL_ERROR) && kick)		// calculates the aceptable error to kick
			{
				MQTTClient->publish(playerList[0]->robotId  + "/kicker/kick/cmd", message);
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