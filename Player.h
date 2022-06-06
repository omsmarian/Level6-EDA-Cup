#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <raylib.h>
#include "MQTTClient2.h"
#include <raymath.h>
#include <array>

#define DELTA_GRADIENT 0.01f

using namespace std;

enum PlayerState
{
	GoingToBall,
	AtBall,
	DribblingBall,
	PassingBall,
	KickingBall,
	Free,
	Defending,
	Still
};

const float ROBOT_RADIUS = 0.09;

class Player
{
public:
	Player(string robotIndex, char teamNumber, MQTTClient2 &MQTTClient);
	~Player();
	void moveToSetpoint(vector<float> setPoint);
	vector<float> getSetpoint(Vector3 destination);
	float getSetAngle(Vector3 destination);
	void moveToBall();
	void remove();
	float getKickerPower(Vector3 destination);
	void updateState();
	void passBall(Vector3 friendPos);
	bool isPathBlocked(Vector3 nextPos);
	bool getTotalribblingDistance();
	void findNextPlayer();
	bool isPassPossible(Vector3 friendPos);
	bool isGoalPossible();
	void findNextPos();
	bool isEnemyWithBall();

	Vector3 playerPos, ballPos, dribblingStartPos, nextPos;
	float ballHeight;
	string robotId;
	int teamNum;
	Vector3 gradient;
	vector<float> directionToMove;

	bool kick, enemyHasBall, passIsPossible;
	uint8_t playerState = Still, playerWithBall, nextPlayer;
	array<Vector3, 6> teamPos, enemyPos;

private:
	MQTTClient2 *MQTTClient;
	Image image;
	uint8_t timer = 0;
	bool update = true;
	float getCost(Vector3 position, Vector3 center);
	Vector3 GetDirection(Vector3 position);

};

#endif