#include <vector>
#include <raylib.h>
#include "MQTTClient2.h"
#include <raymath.h>
#include <array>

using namespace std;

enum TeamState
{
	Attacking,
	Defending,
	Nothing
};

enum PlayerState
{
	GoingToBall,
	AtBall,
	DribblingBall,
	PassingBall,
	KickingBall,
	Free,
	Blocking,
	KeepingFormation
};

enum PlayerType
{
	Striker,
	Defender,
	GoalKeeper
};

struct Formation // better with tree? (could also contain positions)
{
	uint8_t Strikers[2];   // 1 2
	uint8_t Middle[1];	   //  1
	uint8_t Defenders[2];  // 1 2
	uint8_t GoalKeeper[1]; //  1
};

const Vector2 baseFormation[] = {{-1, 0}, {-2, 1}, {-2, -1}, {-3.5, 2}, {-3.5, 0}, {-3.5, -2}}; // GameController?
// maybe relative positions are better

const float ROBOT_RADIUS = 0.09;
const uint8_t NOBODY = -1;

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
	bool getTotalDribblingDistance();
	void findNextPlayer();
	bool isShotPossible(Vector2 position), isPassPossible(Vector2 friendPos), isGoalPossible();
	void findNextPos();
	bool isPlayerClosestTo(Vector2 position);
	uint8_t findClosestPlayerTo(Vector2 position);
	void keepFormation();
	void kickBall(float voltage);
	bool isPlayerFacingGoal();
	void findPosition();

	bool enemyHasBall, passIsPossible;
	string robotId;
	uint8_t playerNum, playerWithBall = NOBODY, nextPlayer, teamNum, teamSize;
	Vector2 playerPos, ballPos, dribblingStartPos, nextPos, formationPos;
	float ballHeight;
	TeamState teamState = Nothing;
	PlayerState playerState = Free;
	Formation formation;
	PlayerType playerType = Defender; // should be initialized by constructor
	array<Vector2, 6> teamPos, enemyTeamPos;

private:
	MQTTClient2 *MQTTClient;
	Image image;
	uint8_t timer = 0;
	bool update = false;
};
