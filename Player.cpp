#include "Player.h"
#include "math.h"

#define MIN_DISTANCE 0.01
#define CHARGE_VALUE 200
#define MINIMAL_ERROR 10
#define DELTA_ANGLE 28
#define DELTA_DISTANCE 1

Player::Player(string robotIndex, MQTTClient2 &MQTTClient)
{

	robotId = "robot1." + robotIndex;

	this->MQTTClient = &MQTTClient;
	ballPos.resize(3);
	playerRot.resize(3);
	playerPos.resize(3);

    image = LoadImage("../Images/image.png");
	ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);

	Rectangle selectRectangle = { 16.0F * (stoi(robotIndex)-1) , 0, 16, 16 };
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

/**
 * @brief Calculates angle from the points given
 * @param start, first point
 * @param finish, second point
 * @return Angle
 */
float Player::angleCalculator(vector<float> start, vector<float> finish)
{
	float leg1 = finish[0] - start[0];
	float leg2 = finish[2] - start[2];
	float angle = atan2(leg1, leg2) * 180 / PI;
	if (angle < 0)
		angle += 360;
	return angle;
}

/**
 * @brief Moves the robot to a position
 * @param setPoint, the point where the robot will go
 */
void Player::moveRobotToSetPoint(vector<float> setPoint)
{
	vector<char> mensaje(12);
	memcpy(mensaje.data(), setPoint.data(), 3 * sizeof(float));
	MQTTClient->publish("robot1.1/pid/setpoint/set", mensaje);
}

/**
 * @brief given the end position, divide the path into intervals and return the next position
 * @param destinationPoint
 * @return next position
 */
vector<float> Player::getSetPoint(vector<float> destinationPoint)
{
	Vector2 deltaVector = { destinationPoint[0] - playerPos[0],
		destinationPoint[2] - playerPos[2] };
	Vector2 currentPosition = { playerPos[0], playerPos[2] };
	Vector2 destination = { destinationPoint[0] , destinationPoint[2] };
	float distance = Vector2Distance(currentPosition, destination);

	if (distance > DELTA_DISTANCE)
		destination = Vector2Add(currentPosition, Vector2Scale(deltaVector, DELTA_DISTANCE / distance));

	vector<float> setPoint = { destination.x, destination.y,
		angleCalculator(playerPos, destinationPoint) };
	return setPoint;
}

/**
 * @brief given the final angle, divide the path into intervals and return the next angle
 * @param destination
 * @return next angle
 */
float Player::getSetAngle(vector<float> destination)
{
	float initialAngle = angleCalculator(playerPos, ballPos);
	float targetAngle = angleCalculator(playerPos, destination);

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