/*
 * Main
 *
 * 22.08 EDA
 *
 * Grupo 6
 * Participants: Segundo Martinez Tanoira, Mariano OMS, Lucia Ruiz
 *
 * Level 6:
 *
 * Main module
 */
#include <iostream>
#include <raylib.h>
#include "MQTTClient2.h"
#include "MQTTListener2.h"

using namespace std;

int main(void)
{
    MQTTClient2 robot;
    robot.connect("robot1.1", "127.0.0.1", 1883, "user", "vdivEMMN3SQWX2Ez");
    if (robot.isConnected())
        cout << "hola" << endl;
    robot.subscribe("ball/motion/state");
    
    MQTTListener2 listener;
    robot.setListener(&listener);

}