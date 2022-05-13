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

#define ADDRESS "127.0.0.1"
#define PORT 1883
#define USER "user"
#define PASSWORD "vdivEMMN3SQWX2Ez"

using namespace std;

int main(void)
{
    MQTTListener2 listener;
    MQTTClient2 robot;
    robot.setListener(&listener);
    robot.connect("robot1.1", ADDRESS, PORT, USER, PASSWORD);
    robot.subscribe("ball/motion/state");
    robot.run();
    if(!robot.isConnected())
    {
        cout << "error connecting" << endl;
    }

    
    

    return 0;
}