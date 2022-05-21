#ifndef GameController_h
#define GameController_h

#include "MQTTClient2.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <utility>
/**
 * @file GameController.h
 * @author Grupo 6: Mariano Ohms, Segundo Tanoira, Lucia Ruiz, Valentin Vieira
 * @brief Controls the game
 * @date 2022-05-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
	GameController(MQTTClient2* mqtt);
	~GameController();
	void onMessage(std::string topic, std::vector<char> payload);


private:
	float angleCalculator(std::vector<float> vector1, std::vector<float> vector2);
	void moveRobotToSetPoint(vector<float> setPoint);
	vector<float> getSetPoint(vector<float>);
	float getSetAngle(vector<float> destination);
	void kickToGoal();

	int playerState;
	bool change;
	int timer;
	Image image;

	std::vector<char> lastPayload;
	MQTTClient2* MQTTClient;
	std::vector<float> ballPos;
	std::vector<float> ballVel;
	std::vector<float> playerPos;
	std::vector<float> playerRot;
	float vel;
	bool kick = true;
};




#endif // !GameController_h
