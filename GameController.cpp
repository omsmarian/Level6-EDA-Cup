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
#include <cstring>

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
GameController::GameController(MQTTClient2 *mqtt, vector<Player *> auxList)
{
	MQTTClient = mqtt;
	for (int i = 5; i >= 0; i--)
	{
		playerList.push_back(auxList[i]);
	}
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
		setInitialPositions();
		playerList[0]->moveToBall();
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

	for (auto player : playerList)
	{
		setInitialPositions();
		memcpy(player->teamPos.data(), teamPos.data(), teamSize * sizeof(Vector2));
		memcpy(player->enemyPos.data(), enemyPos.data(), teamSize * sizeof(Vector2));
		player->ballPos = ballPos;
		player->ballHeight = ballHeight;
		player->updateState();
	}
}

/**
 * @brief
 *
 * @param topic
 * @param payload
 */
void GameController::recieveInformation(string topic, vector<char> payload)
{
	for (uint8_t i = 0; i < teamSize; i++)
	{
		Vector2 aux = {*((float *)&payload[0]), *((float *)&payload[8])};
		if (topic == (playerList[i]->robotId + "/motion/state"))
			playerList[i]->playerPos = teamPos[i] = aux;
		else if (topic == ("robot2." + to_string(i) + "/motion/state"))
			enemyPos[i] = aux;
	}
	if (topic == "ball/motion/state")
	{
		if (lastPayload != payload)
		{
			ballPos = ballPos = {*((float *)&payload[0]), *((float *)&payload[8])};
			ballHeight = *((float *)&payload[4]);
			lastPayload = payload;
		}
	}
	else if (topic == "edacup/preKickOff")
	{
		std::cout << "preKickOFF" << std::endl;
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));
		gameState = preKickOff;
	}
	else if (topic == "edacup/kickOff")
	{
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));
		gameState = kickOff;
	}
	else if (topic == "edacup/preFreeKick")
	{
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));
		gameState = preFreeKick;
	}
	else if (topic == "edacup/freeKick")
	{
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));
		gameState = freeKick;
	}
	else if (topic == "edacup/prePenaltyKick")
	{
		std::cout << "prePenaltyKick" << std::endl;
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));
		gameState = prePenaltyKick;
	}
	else if (topic == "edacup/penaltyKick")
	{
		std::cout << "penaltyKick" << std::endl;
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));
		gameState = penaltyKick;
	}
	else if (topic == "edacup/pause")
	{
		gameState = pauseGame;
	}
	else if (topic == "edacup/continue")
	{
		gameState = continueGame;
	}
	else if (topic == "edacup/removeRobot")
	{
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));

		if (teamMessageRefersTo == playerList[0]->teamNum)
		{
			gameState = removeRobot;
		}
	}
	else if (topic == "edacup/addRobot")
	{
		memcpy(&teamMessageRefersTo, &payload, sizeof(uint8_t));

		if (teamMessageRefersTo == playerList[0]->teamNum)
		{
			gameState = addRobot;
		}
	}
}

void GameController::setInitialPositions()
{
	if (teamMessageRefersTo != teamNumber)
		playerList[0]->moveToSetpoint(playerList[0]->getSetpoint({-1, 0}));
	else
	{
	}

	// const Vector2 positions[] = {{-2, 1}, {-2, -1}, {-3.5, 2}, {-3.5, 0}, {-3.5, -2}};
	// for (uint8_t i = 1; i < teamSize; i++)
	// 	playerList[i]->moveToSetpoint(playerList[i]->getSetpoint(positions[i]));

	playerList[1]->moveToSetpoint(playerList[0]->getSetpoint({-2, 1}));
	playerList[2]->moveToSetpoint(playerList[0]->getSetpoint({-2, -1}));
	playerList[3]->moveToSetpoint(playerList[0]->getSetpoint({-3.5, 2}));
	playerList[4]->moveToSetpoint(playerList[0]->getSetpoint({-3.5, 0}));
	playerList[5]->moveToSetpoint(playerList[0]->getSetpoint({-3.5, -2}));
}
