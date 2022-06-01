#include "Player.h"
#include "math.h"
#include <iostream>
#include <cstring>

using namespace std;

#define MIN_DISTANCE 0.01
#define CHARGE_VALUE 200
#define MINIMAL_ERROR 10
#define DELTA_ANGLE 28
#define DELTA_DISTANCE 1


const Vector2 goal = {-4.5, 0};

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
	cout << robotId << endl;
	this->MQTTClient = &MQTTClient;

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

			if (Vector2Distance(playerPos, ballPos) >= MIN_DISTANCE)
				playerState = GoingToBall;
			else
				playerState = AtBall;

			break;
		}
		case AtBall:
		{
			timer = 1;
			float voltage = 3;
			vector<char> message(4);
			memcpy(&(message[0]), &voltage, sizeof(float));
			MQTTClient->publish(robotId + "/dribbler/voltage/set", message);

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
			if (Vector2Distance(playerPos, ballPos) < MIN_DISTANCE)
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
			float ballAngle = 90.0f - Vector2Angle(playerPos, ballPos);
			float goalAngle = 90.0f - Vector2Angle(playerPos, goal);

			if ((abs(goalAngle - ballAngle) < MINIMAL_ERROR) && kick) // calculates the aceptable error to kick
			{
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
			// moveToSetpoint(getSetpoint(Vector2MoveTowards(enemyPos[...])));

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
vector<float> Player::getSetpoint(Vector2 destination)
{
	Vector2 nextPos = Vector2MoveTowards(playerPos, destination, DELTA_DISTANCE);
	return {nextPos.x, nextPos.y, 90.0f - Vector2Angle(playerPos, destination)};
}

/**
 * @brief given the final angle, divide the path into intervals and return the next angle
 * @param destination
 * @return next angle
 */
float Player::getSetAngle(Vector2 destination)
{
	float initialAngle = 90.0f - Vector2Angle(playerPos, ballPos);
	float targetAngle = 90.0f - Vector2Angle(playerPos, destination);
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

float Player::getKickerPower(Vector2 destination)
{
	return 0.25 + 0.225 * logf(1.225 * Vector2Distance(playerPos, destination));
	// return -0.0127 + 0.286 * distance - 0.0455 * pow(distance, 2) + 0.00306 * pow(distance, 3);
}

void Player::passBall(Vector2 friendPos)
{
	const float CHARGE_PER_DELTA_DISTANCE = 50; // charge required to pass the ball to a friend located at delta distance
	float charge = Vector2Distance(playerPos, friendPos) * CHARGE_PER_DELTA_DISTANCE;
}

bool Player::isPathBlocked(Vector2 nextPos)
{
	for (auto enemyPos : this->enemyPos)
		if (CheckCollisionCircles(nextPos, ROBOT_RADIUS, enemyPos, ROBOT_RADIUS))
			return false;
	return true;
}

bool Player::getTotalribblingDistance()
{
	return Vector2Distance(playerPos, dribblingStartPos);
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
bool CheckCollisionCircleLine(Vector2 center, float radius, Vector2 startPos, Vector2 endPos)
{
	Vector2 LineTangent = Vector2Scale(Vector2Normalize(Vector2Subtract(endPos, startPos)), radius);
	Vector2 LineNormal = {-LineTangent.y, LineTangent.x}, aux;
	return CheckCollisionLines(startPos, endPos, Vector2Add(center, LineNormal),
							   Vector2Subtract(center, LineNormal), &aux); // collision point is not important
}

bool Player::isPassPossible(Vector2 friendPos) // should consider enemy movement
{
	for (auto enemyPos : this->enemyPos)
		if (CheckCollisionCircleLine(enemyPos, ROBOT_RADIUS, playerPos, friendPos))
			return false;
	return true;
}

bool Player::isEnemyWithBall()
{
	for (auto enemy : enemyPos)
		if (Vector2Distance(enemy, ballPos) <= ROBOT_RADIUS + MIN_DISTANCE)
			// enemyHasBall = true;
			return true;
	return false;
}
