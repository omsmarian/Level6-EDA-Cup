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
GameController::GameController(MQTTClient2* mqtt, vector<Player*> auxList)
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
	{
		setInitialPositions();
		break;
	}
	case kickOff:
	{
		playerList[0]->moveToBall();
		break;
	}
	case preFreeKick:
	{
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

	for (auto player : playerList)
	{
		memcpy(player->teamPos.data(), teamPos.data(), teamSize * sizeof(Vector2));
		memcpy(player->enemyPos.data(), enemyPos.data(), teamSize * sizeof(Vector2));
		player->ballPos = ballPos;
		player->ballHeight = ballHeight;
		//player->playerState = Still;
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
		if (topic == (playerList[i]->robotId + "/motion/state"))
		{
			Vector2 aux = { *((float*)&payload[0]), *((float*)&payload[8]) };
			playerList[i]->playerPos = teamPos[i] = aux;
		}
		else if (topic == ("robot2." + to_string(i) + "/motion/state"))
		{
			Vector2 aux = { *((float*)&payload[0]), *((float*)&payload[8]) };
			enemyPos[i] = aux;
		}
	}
	if (topic == "ball/motion/state")
	{
		if (lastPayload != payload)
		{
			ballPos = { *((float*)&payload[0]), *((float*)&payload[8]) };
			ballHeight = *((float*)&payload[4]);
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
	playerList[0]->moveToSetpoint(playerList[0]->getSetpoint({ -1, 0 }));
	playerList[1]->moveToSetpoint(playerList[1]->getSetpoint({ -2, 1 }));
	playerList[2]->moveToSetpoint(playerList[2]->getSetpoint({ -2, -1 }));
	playerList[3]->moveToSetpoint(playerList[3]->getSetpoint({ -3.5, 2 }));
	playerList[4]->moveToSetpoint(playerList[4]->getSetpoint({ -3.5, 0 }));
	playerList[5]->moveToSetpoint(playerList[5]->getSetpoint({ -3.5, -2 }));
}


GridWithWeights GameController::makeHeatMap() {
  GridWithWeights grid(90, 60);
  typedef GridLocation L;

  grid.forests.emplace(L{1,0,3});     // construye un "nodo" con el peso correspondiente en el set
  
  return grid;
}


GridLocation GameController::assignWeightToHeatMapLocation(vector<float> actualPosition)
{
	// vos le pasas una posicion y te fijas si esta cerca de un robot, te fijas cuan cerca esta
	// y le asignas el peso correspondiente
}


GridLocation GameController::actualPositionToHeatMapPosition(vector<float> actualPosition)
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

	GridLocation heatMapPosition = {auxActualPosition[0], auxActualPosition[1]};
	
    return heatMapPosition;
}


vector<float> GameController::heatMapPositionToActualPosition(GridLocation heatMapLocation)
{

}

