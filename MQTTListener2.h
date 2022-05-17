#ifndef MQTTListener2_h
#define MQTTListener2_h

#include "MQTTClient2.h"

enum PlayerState // estado de juego
{
    goingToBall,
	atBall,
	inArea
};

class MQTTListener2 : public MQTTListener
{
public:
	MQTTListener2(MQTTClient2* mqtt);
	void onMessage(std::string topic, std::vector<char> payload);

private:
	void printVector(std::vector<float> vector);
	float angleCalculator(std::vector<float> vector1, std::vector<float> vector2);
	void setRobotDestinationPoint(bool change);
	
	//pair<int, int> actualPositionToHeatMapPosition(vector<float> actualPosition);
	void kickToGoal();

	int playerState;

	std::vector<char> lastPayload;
	MQTTClient2* miau;
	std::vector<float> ballPos;
	std::vector<float> ballVel;
	std::vector<float> ballRot;
	std::vector<float> ballAngVel;
	std::vector<float> playerPos;
};




#endif // !MQTTListener2_h
