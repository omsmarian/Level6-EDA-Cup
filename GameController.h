#ifndef GameController_h
#define GameController_h

#include "MQTTClient2.h"
#include <iostream>
#include <raylib.h>
#include <math.h>
#include <utility>
#include <raymath.h>
#include <vector>

using namespace std;

enum PlayerState // estado de juego
{
	goingToBall,
	atBall,
	inArea,
	none
};

class GameController : public MQTTListener
{
public:
	GameController(MQTTClient2* mqtt);
	void onMessage(std::string topic, std::vector<char> payload);

private:
	void printVector(std::vector<float> vector);
	float angleCalculator(std::vector<float> vector1, std::vector<float> vector2);
	void moveRobotToSetPoint(vector<float> setPoint);
	vector<float> getSetPoint(vector<float>);
	float getSetAngle(vector<float> destination);
	
	//pair<int, int> actualPositionToHeatMapPosition(vector<float> actualPosition);
	void kickToGoal();

	int playerState;
	bool change;
	int timer;

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
