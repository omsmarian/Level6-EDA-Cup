#include "Player.h"
#include "math.h"
#include <iostream>
#include <cstring>

using namespace std;

#define MIN_DISTANCE 0.1
#define CHARGE_VALUE 200
#define MINIMAL_ERROR 10
#define DELTA_ANGLE 28
#define DELTA_DISTANCE 0.7


Player::Player(string robotIndex, char teamNumber, MQTTClient2 &MQTTClient)
{
	if (teamNumber == '1')
	{
		teamNum = 1;
		robotId = "robot1." + robotIndex;
	}
	else if (teamNumber == '2')
	{
		teamNum = 2;
		robotId = "robot2." + robotIndex;
	}
	this->MQTTClient = &MQTTClient;

	if(teamNumber == 1)
	{
		goal = {4.5, 0, 0.4};
	}
	else if(teamNumber == 2)
	{
		goal = {-4.5, 0, 0.4};
	}

	image = LoadImage("../Images/image.png");

	ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);

	Rectangle selectRectangle = {16.0F * (stoi(robotIndex) - 1), 0, 16, 16};
	Image selectedImage = ImageFromImage(image, selectRectangle);

	const int dataSize = 16 * 16 * 3;
	vector<char> payload(dataSize);
	memcpy(payload.data(), selectedImage.data, dataSize);

	UnloadImage(selectedImage);

	MQTTClient.publish(robotId + "/display/lcd/set", payload);
}

Player::~Player()
{
	UnloadImage(image);
}

void Player::updateState()
{
	if (update)
	{
		switch (playerState)
		{
		case GoingToBall:
		{
			timer = 3;
			moveToBall();
			cout << "going to ball" << endl;

			if (Vector3Distance(playerPos, ballPos) >= MIN_DISTANCE)
				playerState = GoingToBall;
			else
				playerState = AtBall;

			break;
		}
		case AtBall:
		{
			cout << " at ball " << endl;
			timer = 1;
			float voltage = 3;
			vector<char> message(4);
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish(robotId + "/dribbler/voltage/set", message);

			playerState = KickingBall;
			dribblingStartPos = playerPos;

			if (ballHeight > 0.1)
				playerState = DribblingBall;

			break;
		}
		case DribblingBall:
		{
			timer = 1;
			vector<float> setpoint = {playerPos.x, playerPos.y, getSetAngle(goal)};
			findNextPos();
			if (Vector3Distance(playerPos, ballPos) < MIN_DISTANCE)
			{
				if (isGoalPossible())
					playerState = KickingBall;
				else if (isPathBlocked(nextPos))
				{ // setPoint = {..., playerList[0]->getSetAngle(findNextPlayer())};
				}
				else
					setpoint = getSetpoint(goal);
			}
			else
				playerState = GoingToBall;

			moveToSetpoint(setpoint);

			break;
		}
		case PassingBall:
		{
			findNextPlayer();
			if (isPassPossible(teamPos[nextPlayer]))
			{
				// passBall(nextPlayer);
				playerState = Free;
			}
			else
				playerState = DribblingBall;

			break;
		}
		case KickingBall:
		{
			float ballAngle = 90.0f - Vector2Angle({playerPos.x, playerPos.y}, {ballPos.x, ballPos.y});
			float goalAngle = 90.0f - Vector2Angle({playerPos.x, playerPos.y}, {goal.x, goal.y});

			if ((abs(goalAngle - ballAngle) < MINIMAL_ERROR)) // calculates the aceptable error to kick
			{
				cout << "kicking ball" << endl;
				// voltage = getKickerPower(goal);
				const float MAX_POWER = 0.8f;
				float voltage = MAX_POWER;
				vector<char> message(4);
				*((float *)&message[0]) = voltage;
				MQTTClient->publish(robotId + "/kicker/kick/cmd", message);
				// kick = false;
			}
			else
				playerState = DribblingBall;

			break;
		}
		case Free:
		{
			if (!isPassPossible(teamPos[nextPlayer]))
			{
				// findNextPos();
			}

			break;
		}
		case Defending:
		{
			// moveToSetpoint(getSetpoint(Vector3MoveTowards(enemyPos[...])));

			break;
		}
		case Still:
		{
			// Do nothing

			break;
		}
		}
		update = false;
	}

	(timer <= 0) ? (update = true) : (timer--);
}

/**
 * @brief Moves the robot to a position
 * @param setPoint, the point where the robot will go
 */
void Player::moveToSetpoint(vector<float> setpoint)
{
	vector<char> mensaje(12);
	memcpy(mensaje.data(), setpoint.data(), 3 * sizeof(float));
	MQTTClient->publish(robotId + "/pid/setpoint/set", mensaje);
}

/**
 * @brief given the end position, divide the path into intervals and return the next position
 * @param destinationPoint
 * @return next position
 */
vector<float> Player::getSetpoint(Vector3 destination)
{
	// Vector2 auxnextPos = Vector2MoveTowards({playerPos.x, playerPos.y}, {destination.x, destination.y}, DELTA_DISTANCE);
	// Vector3 nextPos = Vector3Add(Vector3Scale(GetDirection(Vector3{auxnextPos.x, 0, auxnextPos.y}), 2), Vector3{auxnextPos.x, 0, auxnextPos.y});
	// return {nextPos.x, nextPos.z, 90.0f - Vector2Angle({playerPos.x, playerPos.y}, {destination.x, destination.y})};
	Vector2 nextPos = Vector2MoveTowards({playerPos.x, playerPos.y}, {destination.x, destination.y}, DELTA_DISTANCE);
	return {nextPos.x, nextPos.y, 90.0f - Vector2Angle({playerPos.x, playerPos.y}, {destination.x, destination.y})};
}

/**
 * @brief given the final angle, divide the path into intervals and return the next angle
 * @param destination
 * @return next angle
 */
float Player::getSetAngle(Vector3 destination)
{
	float initialAngle = 90.0f - Vector2Angle({playerPos.x, playerPos.y}, {ballPos.x, ballPos.y});
	float targetAngle = 90.0f - Vector2Angle({playerPos.x, playerPos.y}, {destination.x, destination.y});
	float substraction = initialAngle - targetAngle;
	if (abs(substraction) > DELTA_ANGLE)
	{
		if (abs(substraction) < 180)
			return (initialAngle - (DELTA_ANGLE * ((substraction > 0) - (substraction < 0))));
		else
			return (initialAngle + (DELTA_ANGLE * ((substraction > 0) - (substraction < 0))));
	}
	else
		return targetAngle;
}


void Player::moveToBall()
{
	vector<char> voltage(4, 0);
	MQTTClient->publish(robotId + "/dribbler/voltage/set", voltage);
	moveToSetpoint(getSetpoint(ballPos));
	*((float *)&voltage[0]) = CHARGE_VALUE;
	MQTTClient->publish(robotId + "/kicker/chargeVoltage/set", voltage);
}

void Player::remove()
{
}

float Player::getKickerPower(Vector3 destination)
{
	return 0.25 + 0.225 * logf(1.225 * Vector3Distance(playerPos, destination));
	// return -0.0127 + 0.286 * distance - 0.0455 * pow(distance, 2) + 0.00306 * pow(distance, 3);
}

void Player::passBall(Vector3 friendPos)
{
	const float CHARGE_PER_DELTA_DISTANCE = 50; // charge required to pass the ball to a friend located at delta distance
	float charge = Vector3Distance(playerPos, friendPos) * CHARGE_PER_DELTA_DISTANCE;
}

bool Player::isPathBlocked(Vector3 nextPos)
{
	for (auto enemyPos : this->enemyPos)
		if (CheckCollisionCircles({nextPos.x, nextPos.y}, ROBOT_RADIUS, {enemyPos.x, enemyPos.y}, ROBOT_RADIUS))
			return false;
	return true;
}

bool Player::getTotalribblingDistance()
{
	return Vector3Distance(playerPos, dribblingStartPos);
}

void Player::findNextPlayer()
{
	// nextPlayer = ...
}

void Player::findNextPos()
{
	// nextPos = ...
}

bool Player::isGoalPossible()
{
	return true;
}

// Check collision between circle and line
bool CheckCollisionCircleLine(Vector3 center, float radius, Vector3 startPos, Vector3 endPos)
{
	Vector2 LineTangent = Vector2Scale(Vector2Normalize(Vector2Subtract({endPos.x, endPos.y}, {startPos.x, startPos.y})), radius);
	Vector2 LineNormal = {-LineTangent.y, LineTangent.x}; 
	Vector2 aux;
	return CheckCollisionLines({startPos.x, startPos.y}, {endPos.x, endPos.y}, Vector2Add({center.x, center.y}, LineNormal),
							   Vector2Subtract({center.x, center.y}, LineNormal), &aux); // collision point is not important
}

bool Player::isPassPossible(Vector3 friendPos) // should consider enemy movement
{
	for (auto enemyPos : this->enemyPos)
		if (CheckCollisionCircleLine(enemyPos, ROBOT_RADIUS, playerPos, friendPos))
			return false;
	return true;
}

bool Player::isEnemyWithBall()
{
	for (auto enemy : enemyPos)
		if (Vector3Distance(enemy, ballPos) <= ROBOT_RADIUS + MIN_DISTANCE)
			// enemyHasBall = true;
			return true;
	return false;
}

float Player::getCost(Vector3 position, Vector3 center)
{
	float sigma = 1.5;
	float cost = expf(-(Vector3Distance(position, center)*Vector3Distance(position, center))/(sigma * sigma));
	return cost;
}


Vector3 Player::GetDirection(Vector3 position)
{	float cost = 0, costX = 0, costZ = 0;
	Vector3 gradient;
	for(int i = 0; i<=5; i++)
	{
		if(Vector3Distance(enemyPos[i], position) < 1)
		{
			cost += getCost(position, enemyPos[i]); 
			costX += getCost(position, Vector3Add(enemyPos[i], Vector3{ DELTA_GRADIENT , 0, 0 })); 
			costZ += getCost(position, Vector3Add(enemyPos[i], Vector3{ 0, 0, DELTA_GRADIENT }));
		}
	}
	gradient = {(costX - cost)/DELTA_GRADIENT, 0, (costZ - cost)/DELTA_GRADIENT};
	gradient = Vector3Normalize(Vector3Scale((gradient), -1));
	cout << "gradient: " << gradient.x << " , " << gradient.y << " , " << gradient.z << endl;

	// vector <float> directionToMove = {auxGradient.x, auxGradient.y, auxGradient.z};
	// cout << directionToMove[0] << " , " << directionToMove[1] << " , " << directionToMove[2] << endl;

	return gradient;
}