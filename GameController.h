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

using namespace std;

enum PlayerState
{
	goingToBall,
	atBall
};

class GameController : public MQTTListener
{
public:

	GameController(MQTTClient2* mqtt, vector<Player*> playerList);
	~GameController();
	void onMessage(std::string topic, std::vector<char> payload);


private:

/*	float angleCalculator(std::vector<float> vector1, std::vector<float> vector2);
	void moveRobotToSetPoint(vector<float> setPoint);
	vector<float> getSetPoint(vector<float>);
	float getSetAngle(vector<float> destination);*/

	uint8_t playerState;
	uint8_t timer;
	//Image image;

	MQTTClient2* MQTTClient;
	vector<Player*> playerList;
	
	std::vector<char> lastPayload;
	/*std::vector<float> ballPos;
	std::vector<float> playerPos;
	std::vector<float> playerRot;*/
	
	bool kick;
	bool update;
};




#endif // !GameController_h
