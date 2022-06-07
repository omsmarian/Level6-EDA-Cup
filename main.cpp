/**
 * @file main.cpp
 * @author Grupo 6: Mariano Ohms, Segundo Tanoira, Lucia Ruiz, Valentin Vieira
 * @brief main module
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "MQTTClient2.h"
#include "GameController.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{


	MQTTClient2 MQTTClient;

	MQTTClient.connect("controller", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");

	//char teamNumber = *argv[1];
	char teamNumber = '1';
	char enemyTeamNumber;
	if(teamNumber == '1')
	{
		char enemyTeamNumber = '2';
	}
	else
	{
		char enemyTeamNumber = '1';
	}

	Player robot1("1", teamNumber, MQTTClient);
	Player robot2("2", teamNumber, MQTTClient);
	Player robot3("3", teamNumber, MQTTClient);
	Player robot4("4", teamNumber, MQTTClient);
	Player robot5("5", teamNumber, MQTTClient);
	Player robot6("6", teamNumber, MQTTClient);
	Player robot7("1", enemyTeamNumber, MQTTClient);
	Player robot8("2", enemyTeamNumber, MQTTClient);
	Player robot9("3", enemyTeamNumber, MQTTClient);
	Player robot10("4", enemyTeamNumber, MQTTClient);
	Player robot11("5", enemyTeamNumber, MQTTClient);
	Player robot12("6", enemyTeamNumber, MQTTClient);


	vector<Player *> playerList;
	// vector<Player *> playerList = {&robot1, &robot2, &robot3, &robot4, &robot5, &robot6};

	playerList.push_back(&robot12);
	playerList.push_back(&robot11);
	playerList.push_back(&robot10);
	playerList.push_back(&robot9);
	playerList.push_back(&robot8);
	playerList.push_back(&robot7);
	playerList.push_back(&robot6);
	playerList.push_back(&robot5);
	playerList.push_back(&robot4);
	playerList.push_back(&robot3);
	playerList.push_back(&robot2);
	playerList.push_back(&robot1);

	GameController listener(&MQTTClient, playerList);
	MQTTClient.setListener(&listener);

	MQTTClient.subscribe("ball/motion/state");
	for (uint8_t i = 1; i < 3; i++)
	{
		for (uint8_t j = 1; j < 7; j++)
		{
			MQTTClient.subscribe("robot" + to_string(i) + "." + to_string(j) + "/motion/state");
		}
	}

	MQTTClient.subscribe("edacup/preKickOff");
	MQTTClient.subscribe("edacup/kickOff");
	MQTTClient.subscribe("edacup/preFreeKick");
	MQTTClient.subscribe("edacup/freeKick");
	MQTTClient.subscribe("edacup/prePenaltyKick");
	MQTTClient.subscribe("edacup/penaltyKick");
	MQTTClient.subscribe("edacup/pause");
	MQTTClient.subscribe("edacup/continue");
	MQTTClient.subscribe("edacup/removeRobot");
	MQTTClient.subscribe("edacup/addRobot");

	MQTTClient.run();

	return 0;
}