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
	// if (argc != 2)
	// 	return -1;

	MQTTClient2 MQTTClient;

	MQTTClient.connect("controller", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");

	// char teamNumber = *argv[1];
	char teamNumber = '1';

	Player robot1("1", teamNumber, MQTTClient);
	Player robot2("2", teamNumber, MQTTClient);
	Player robot3("3", teamNumber, MQTTClient);
	Player robot4("4", teamNumber, MQTTClient);
	Player robot5("5", teamNumber, MQTTClient);
	Player robot6("6", teamNumber, MQTTClient);

	vector<Player *> playerList;
	// vector<Player *> playerList = {&robot1, &robot2, &robot3, &robot4, &robot5, &robot6};

	playerList.push_back(&robot6);
	playerList.push_back(&robot5);
	playerList.push_back(&robot4);
	playerList.push_back(&robot3);
	playerList.push_back(&robot2);
	playerList.push_back(&robot1);

	GameController listener(&MQTTClient, playerList);
	MQTTClient.setListener(&listener);

	MQTTClient.subscribe("ball/motion/state");
	for (uint8_t i = 0; i < 2; i++)
		for (uint8_t j = 0; j < 6; j++)
			MQTTClient.subscribe("robot" + to_string(i) + "." + to_string(j) + "/motion/state");
	MQTTClient.subscribe("edacup/preKickOff");
	MQTTClient.subscribe("edacup/kickOff");
	MQTTClient.subscribe("edacup/preFreeKick");
	MQTTClient.subscribe("edacup/freeKick");
	MQTTClient.subscribe("edacup/prePenaltyKick");

	MQTTClient.run();

	return 0;
}