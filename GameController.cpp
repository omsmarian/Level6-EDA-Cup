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
#include <unordered_map>


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
GameController::GameController(MQTTClient2 *mqtt, vector<Player *> playersList)
{
	MQTTClient = mqtt;
	for (int i = 11; i >= 0; i--)
	{
		playerList.push_back(playersList[i]);
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
	Vector3 aux;
	if(update)
	{
		switch (gameState)
		{
			case preKickOff:
			{
				timer = 3;
				setInitialPositions();
				break;
			}
			case kickOff:
			{
				cout << "kickoff" << endl;
				//playerList[0]->moveToBall();
				break;
			}
			case preFreeKick:
			{
				Vector3 aux = {4,0,2};
				playerList[0]->moveToSetpoint(GetDirection(aux, playerList[0]->playerPos));
					//playerList[0]->moveToSetpoint(playerList[0]->getSetpoint({destination[0], destination[1]}));
					//playerList[0]->moveToSetpoint(playerList[0]->getSetpoint({-1, 0}));
				break;
			}
			case freeKick:
			{
				break;
			}
			case prePenaltyKick:
			{
				break;
			}
			case penaltyKick:
			{
				break;
			}
			case pauseGame:
			{
				break;
			}
			case continueGame:
			{
				break;
			}
			case removeRobot:
			{
				break;
			}
			case addRobot:
			{
				break;
			}
		}
	}

	for (auto player : playerList)
	{
		memcpy(player->teamPos.data(), teamPos.data(), teamSize * sizeof(Vector3));
		memcpy(player->enemyPos.data(), enemyPos.data(), teamSize * sizeof(Vector3));
		player->ballPos = ballPos;
		player->ballHeight = ballHeight;
		// player->playerState = Still;
		player->updateState();
	}
	
	if (timer <= 0)
	{
		update = true;
	}
	if (timer > 0)
	{
		update = false;
		timer--;
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
		if (topic == (playerList[i]->robotId + "/motion/state"))
		{
			Vector3 aux = {*((float *)&payload[0]), *((float *)&payload[8])};
			playerList[i]->playerPos = teamPos[i] = aux;
		}
		else if (topic == ("robot2." + to_string(i) + "/motion/state"))
		{
			Vector3 aux = {*((float *)&payload[0]), *((float *)&payload[8])};
			enemyPos[i] = aux;
		}
	}
	if (topic == "ball/motion/state")
	{
		if (lastPayload != payload)
		{
			ballPos = {*((float *)&payload[0]), *((float *)&payload[8])};
			ballHeight = *((float *)&payload[4]);
			lastPayload = payload;
		}
	}
	else if (topic == "edacup/preKickOff")
	{
		cout << "preKickOFF" << endl;
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));
		gameState = preKickOff;
	}
	else if (topic == "edacup/kickOff")
	{
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));
		gameState = kickOff;
	}
	else if (topic == "edacup/preFreeKick")
	{
		cout << "preFreeKick" << endl;
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));
		gameState = preFreeKick;
	}
	else if (topic == "edacup/freeKick")
	{
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));
		gameState = freeKick;
	}
	else if (topic == "edacup/prePenaltyKick")
	{
		cout << "prePenaltyKick" << endl;
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));
		gameState = prePenaltyKick;
	}
	else if (topic == "edacup/penaltyKick")
	{
		cout << "penaltyKick" << endl;
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));
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
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));

		if (teamMessageRefersTo == playerList[0]->teamNum)
		{
			gameState = removeRobot;
		}
	}
	else if (topic == "edacup/addRobot")
	{
		memcpy(&teamMessageRefersTo, payload.data(), sizeof(uint8_t));

		if (teamMessageRefersTo == playerList[0]->teamNum)
		{
			gameState = addRobot;
		}
	}
}

void GameController::setInitialPositions()
{

	playerList[0]->moveToSetpoint(playerList[0]->getSetpoint({-1, 0}));
	playerList[1]->moveToSetpoint(playerList[1]->getSetpoint({-2, 1}));
	playerList[2]->moveToSetpoint(playerList[2]->getSetpoint({-2, -1}));
	playerList[3]->moveToSetpoint(playerList[3]->getSetpoint({-3.5, 2}));
	playerList[4]->moveToSetpoint(playerList[4]->getSetpoint({-3.5, 0}));
	playerList[5]->moveToSetpoint(playerList[5]->getSetpoint({-3.5, -2}));
}


float GameController::getCost(Vector3 position, Vector3 playerPos)
{
	cout << "position " << position.x << " , " << position.y << " , " << position.z << endl;
	cout << "playerPos " << playerPos.x << " , " << playerPos.y << " , " << playerPos.z << endl;
	float distance = Vector3Distance(playerPos, position);
	cout << "distance is" << distance << endl;
	float sigma = 5;
	float cost = expf((distance*distance)/(sigma * sigma));
	return cost;
}

vector<float> GameController::GetDirection(Vector3 position, Vector3 playerPos)
{
	vector<Vector3> gradientList;
	for(auto player : playerList)
	{
		float cost = getCost(position, player->playerPos); 
		float costX = getCost(Vector3Add(position, { DELTA_GRADIENT , 0, 0 }), player->playerPos); 
		float costZ = getCost(Vector3Add(position, { 0, 0, DELTA_GRADIENT }), player->playerPos);
		// cout << cost << endl;
		// cout << costX << endl;
		// cout << costZ << endl;
		
		Vector3 gradient = {-(costX - cost)/DELTA_GRADIENT, 0, -(costZ - cost)/DELTA_GRADIENT};
		gradientList.push_back(gradient);
	}
	
	int index = 0;
	float prev = Vector3Length(gradientList[0]);
	for(int i=1; i < gradientList.size(); i++)
	{
		float actual = Vector3Length(gradientList[i]);
		if(actual > prev)
		{
			prev = actual;
			index = i;
		}
	}

	vector<float> directionToMove(3);
	directionToMove = {gradientList[index].x, gradientList[index].y, gradientList[index].z};


	return directionToMove;
}


