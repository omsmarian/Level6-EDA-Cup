/**
 * @file GameController.h
 * @author Grupo 6: Mariano Ohms, Segundo Tanoira, Lucia Ruiz, Valentin Vieira
 * @brief Controls the game
 * @date 2022-05-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GameController_h
#define GameController_h

#include "MQTTClient2.h"
#include "Player.h"
#include <raylib.h>
#include <math.h>
#include <raymath.h>
#include <vector>
#include <array>



#define DELTA_GRADIENT 0.01f


using namespace std;

enum GameState
{
	preKickOff,
	kickOff,
	preFreeKick,
	freeKick,
	prePenaltyKick,
	penaltyKick,
	pauseGame,
	continueGame,
	removeRobot,
	addRobot,
	none
};

class GameController : public MQTTListener
{
public:
	GameController(MQTTClient2 *mqtt, vector<Player *> playerList);
	~GameController();
	void onMessage(std::string topic, std::vector<char> payload);

private:
	void recieveInformation(string topic, vector<char> payload);
	void setInitialPositions();
	float getCost(Vector3 position);
	void GetDirection(void);

	uint8_t timer;
	Image image;

	MQTTClient2 *MQTTClient;
	vector<Player *> playerList;

	std::vector<char> lastPayload;

	Vector3 ballPos;
	float ballHeight;
	array<Vector3, 6> teamPos, enemyPos;

	int gameState, teamNumber, teamSize = 6;
	uint8_t teamMessageRefersTo;
	bool update = false, enemyHasBall = false;
};

#endif // !GameController_h
