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
#include <iostream>

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
	teamNumber = playerList[0]->teamNum;
	gameState = none;

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
	
	recieveInformation(topic, payload);

	switch (gameState)
	{
		case preKickOff:
			setInitialPlayerPositions();
			playerList[0]->moveRobotToBall();
			break;

		case kickOff:
			break;

		case preFreeKick:
			break;
		
		case freeKick:
			break;

		case prePenaltyKick:
			break;
		
		case penaltyKick:
			break;
		
		case pauseGame:
			break;

		case continueGame:
			break;

		case removeRobot:
			break;

		case addRobot:
			break;
	}

	vector<char> message(4);
	float voltage;

	vector<float> setPoint;
	vector<float> goal = { -4.5, 0 , 0 };

	float ballAngle;
	float goalAngle;
	/*
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
			playerList[0]->moveRobotToBall();

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
		timer--;*/
}


/**
 * @brief 
 * 
 * @param topic 
 * @param payload 
 */
void GameController::recieveInformation(string topic, vector<char> payload)
{
	if (topic == (playerList[0]->robotId + "/motion/state"))
	{
		for (int j = 0; j < 3; j++)
		{
			memcpy(&(playerList[0]->playerPos[j]), &(payload[j * sizeof(float)]), sizeof(float));
		}
	}
	if (topic == "ball/motion/state")
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
	if (topic == "edacup/preKickOff")
	{
		std::cout << "preKickOFF" << std::endl;
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));
		gameState = preKickOff;
	}
	if (topic == "edacup/kickOff")
	{
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));
		gameState = kickOff;
	}
	if (topic == "edacup/preFreeKick")
	{
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));
		gameState = preFreeKick;
	}
	if (topic == "edacup/freeKick")
	{
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));
		gameState = freeKick;
	}
	if (topic == "edacup/prePenaltyKick")
	{
		std::cout << "prePenaltyKick" << std::endl;
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));
		gameState = prePenaltyKick;
	}
	if (topic == "edacup/penaltyKick")
	{
		std::cout << "penaltyKick" << std::endl;
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));
		gameState = penaltyKick;
	}
	if (topic == "edacup/pause")
	{
		gameState = pauseGame;
	}
	if (topic == "edacup/continue")
	{
		gameState = continueGame;
	}
	if (topic == "edacup/removeRobot")
	{
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));

		if(teamMessageRefersTo == playerList[0]->teamNum)
		{
			gameState = removeRobot;
		}
	}
	if (topic == "edacup/addRobot")
	{
		memcpy(&teamMessageRefersTo , &payload , sizeof(uint8_t));

		if(teamMessageRefersTo == playerList[0]->teamNum)
		{
			gameState = addRobot;
		}
	}
}


void GameController::setInitialPlayerPositions()
{
	vector<float> destinationPoint(3);
	if(teamMessageRefersTo != teamNumber)
	{
		destinationPoint[0] = -1;
		destinationPoint[2] = 0;
		playerList[0]->setInitialPosition(destinationPoint);
	}
	destinationPoint[0] = -2;
	destinationPoint[2] = 1;
	playerList[1]->setInitialPosition(destinationPoint);
	destinationPoint[2] = -1; 
	playerList[2]->setInitialPosition(destinationPoint);
	destinationPoint[0] = -3.5;
	destinationPoint[2] = 2;
	playerList[3]->setInitialPosition(destinationPoint);
	destinationPoint[2] = 0;
	playerList[4]->setInitialPosition(destinationPoint);
	destinationPoint[2] = -2;
	playerList[5]->setInitialPosition(destinationPoint);
}