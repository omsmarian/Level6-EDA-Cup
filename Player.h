/**
 * @file GameController.cpp
 * @author Grupo 6: Mariano Ohms, Segundo Tanoira, Lucia Ruiz, Valentin Vieira
 * @brief Controls the Players
 * @date 2022-05-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <raylib.h>
#include "MQTTClient2.h"
#include <raymath.h>
#include <array>
#include "math.h"
#include <iostream>
#include <cstring>

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
	void updateState();
	void moveToSetpoint(vector<float> setPoint);
	vector<float> getSetpoint(Vector3 destination);

	Vector3 playerPos, ballPos, dribblingStartPos, nextPos;
	string robotId;
	int teamNum;
	Vector3 gradient;
	float ballHeight;

	bool kick, enemyHasBall, passIsPossible;
	uint8_t playerState = Still, playerWithBall, nextPlayer;
	array<Vector3, 6> teamPos, enemyPos;

private:
	MQTTClient2 *MQTTClient;
	Image image;
	uint8_t timer = 0;
	bool update = true;
	Vector3 goal;

	Vector3 GetDirection(Vector3 position);
	float getCost(Vector3 position, Vector3 center);
	float getSetAngle(Vector3 destination);
	void moveToBall();
	void remove();
	float getKickerPower(Vector3 destination);
	void passBall(Vector3 friendPos);
	bool isPathBlocked(Vector3 nextPos);
	bool getTotalribblingDistance();
	bool isPassPossible(Vector3 friendPos);
	bool isGoalPossible();
	bool isEnemyWithBall();
};

#endif