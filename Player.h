#include <vector>
#include <raylib.h>
#include "MQTTClient2.h"
#include <raymath.h>

using namespace std;

class Player 
{
public:
    Player(string robotIndex, char teamNumber, MQTTClient2 &MQTTClient);
    ~Player();
    void setInitialPosition(vector<float> destinationPoint);
    float angleCalculator(std::vector<float> vector1, std::vector<float> vector2);
	void moveRobotToSetPoint(vector<float> setPoint);
	vector<float> getSetPoint(vector<float>);
	float getSetAngle(vector<float> destination);
    void moveRobotToBall();
    void removeRobot();

    vector<float> ballPos;
	vector<float> playerPos;
	vector<float> playerRot;
    string robotId;
    int teamNum;

private:
    MQTTClient2* MQTTClient;
    Image image;

};