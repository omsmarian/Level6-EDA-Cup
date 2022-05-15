/*********************************************************************
 * @file   main.cpp
 * @brief  
 * 
 * @author Segundo Tanoira, Lucía Ruiz, Mariano Oms
 *********************************************************************/

#include "MQTTClient2.h"
#include "MQTTListener2.h"
#include <iostream>

using namespace std;

int main(void)
{
    MQTTClient2 robot;
    MQTTListener2 listener(&robot);
    robot.setListener(&listener);
    robot.connect("controller", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");
    if (robot.isConnected())
        cout << "hola" << endl;
    robot.subscribe("ball/motion/state");
    robot.subscribe("robot1.1/motion/state");
    robot.run();

    return 0;
}