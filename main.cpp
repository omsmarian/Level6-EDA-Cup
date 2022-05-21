/**
 * @file main.cpp
 * @author Grupo 6: Mariano Ohms, Segundo Tanoira, Lucia Ruiz, Valentin Vieira
 * @brief 
 * @date 2022-05-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "MQTTClient2.h"
#include "GameController.h"
#include <iostream>

using namespace std;

int main(void)
{
    MQTTClient2 robot;
    robot.connect("controller", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");
    GameController listener(&robot);
    robot.setListener(&listener);
    robot.subscribe("ball/motion/state");
    robot.subscribe("robot1.1/motion/state");
    robot.run();




    return 0;
}