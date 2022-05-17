/*********************************************************************
 * @file   main.cpp
 * @brief  
 * 
 * @author Segundo Tanoira, Luc�a Ruiz, Mariano Oms
 *********************************************************************/

#include "MQTTClient2.h"
#include "GameController.h"
#include <iostream>

using namespace std;

int main(void)
{
    MQTTClient2 robot;
    GameController listener(&robot);
    robot.setListener(&listener);
    robot.connect("controller", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");
    if (robot.isConnected())
        cout << "hola" << endl;
    robot.subscribe("ball/motion/state");
    robot.run();

    return 0;
}