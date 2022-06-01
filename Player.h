#include <vector>
#include <raylib.h>
#include "MQTTClient2.h"
#include <raymath.h>
#include <array>

using namespace std;

enum PlayerState
{
	GoingToBall,
	AtBall,
	DribblingBall,
	PassingBall,
	KickingBall,
	Free,
};

enum TeamState
{
	Attacking,
	Defending,
	Nothing
};

const float ROBOT_RADIUS = 0.09;

class Player
{
public:
	Player(string robotIndex, char teamNumber, MQTTClient2 &MQTTClient);
	~Player();
	void moveToSetpoint(vector<float> setPoint);
	vector<float> getSetpoint(Vector2 destination);
	float getSetAngle(Vector2 destination);
	void moveToBall();
	void remove();
	float getKickerPower(Vector2 destination);
	void updateState();
	void passBall(Vector2 friendPos);
	bool isPathBlocked(Vector2 nextPos);
	bool getTotalribblingDistance();
	void findNextPlayer();
	bool isPassPossible(Vector2 friendPos);
	bool isGoalPossible();
	void findNextPos();

	Vector2 playerPos, ballPos, dribblingStartPos, nextPos;
	float ballHeight;
	string robotId;
	int teamNum;

	bool kick, enemyHasBall, passIsPossible;
	uint8_t playerState, teamState, playerWithBall, nextPlayer;
	array<Vector2, 6> teamPos, enemyPos;

private:
	MQTTClient2 *MQTTClient;
	Image image;
	uint8_t timer = 0;
	bool update = false;
};
