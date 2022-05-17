#ifndef GameController_h
#define GameController_h

#include "MQTTClient2.h"
#include "Algorithm.h"

enum PlayerState // estado de juego
{
    goingToBall,
	atBall,
	inArea
};

class GameController : public MQTTListener
{
public:
	GameController(MQTTClient2* mqtt);
	void onMessage(std::string topic, std::vector<char> payload);

private:
	void printVector(std::vector<float> vector);
	float angleCalculator(std::vector<float> vector1, std::vector<float> vector2);
	void setRobotDestinationPoint(bool change, std::vector<float> &destination);
	std::pair<int, int> actualPositionToHeatMapPosition(std::vector<float> actualPosition);
	void makeHeatMap(int** heatMap);


	int playerState;
	int heatMap[ROW][COL];


	std::vector<char> lastPayload;
	MQTTClient2* robot;
	std::vector<float> ballPos;
	std::vector<float> ballVel;
	std::vector<float> ballRot;
	std::vector<float> ballAngVel;
	std::vector<float> playerPos;
};




#endif // !GameController_h
