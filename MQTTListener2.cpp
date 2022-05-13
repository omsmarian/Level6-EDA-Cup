#include "MQTTListener2.h"
#include <iostream>

using namespace std;

void MQTTListener2::onMessage(string topic, vector<char> payload)
{
    if(topic == "ball/motion/state")
    {
        vector<float> messanger(12);
        for (int i = 0; i < 12; i++)
        {
            memcpy(&(messanger.data()[i]), &(payload.data()[i * sizeof(float)]),sizeof(float));
            cout << messanger[i] << " ," << endl;
        }
    }



    

}


