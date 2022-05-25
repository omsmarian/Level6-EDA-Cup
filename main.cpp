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

using namespace std;

int main(void)
{
    MQTTClient2 MQTTClient;

    MQTTClient.connect("controller", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");

    Player robot1("1", MQTTClient);
    Player robot2("2", MQTTClient);
    Player robot3("3", MQTTClient);
    Player robot4("4", MQTTClient);
    Player robot5("5", MQTTClient);
    Player robot6("6", MQTTClient);

    vector<Player*> playerList;

    playerList.push_back(&robot6);
    playerList.push_back(&robot5);
    playerList.push_back(&robot4);
    playerList.push_back(&robot3);
    playerList.push_back(&robot2);
    playerList.push_back(&robot1);

    GameController listener(&MQTTClient, playerList);
    MQTTClient.setListener(&listener);

    //Player robot2("2");
    //Player robot3("3");
    //Player robot4("4");
    //Player robot5("5");
    //Player robot6("6");

    //list<Player*> playerList;

    //playerList.push_back(&robot1);
    //playerList.push_back(&robot2);
    //playerList.push_back(&robot3);
    //playerList.push_back(&robot4);
    //playerList.push_back(&robot5);
    //playerList.push_back(&robot6);

    MQTTClient.subscribe("ball/motion/state");
    MQTTClient.subscribe("robot1.1/motion/state");

    MQTTClient.run();

    return 0;
}