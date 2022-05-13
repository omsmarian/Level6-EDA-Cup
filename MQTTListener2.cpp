#include "MQTTListener2.h"
#include <iostream>

using namespace std;

void MQTTListener2::onMessage(string topic, vector<char> payload)
{
    if(topic == "ball/motion/state")
    {
        vector<float> message(12);
        for(int i=0; i < 12; i++)
        {
            memcpy(&(message.data()[i]), &(payload.data()[i * sizeof(float)]),sizeof(float));
            cout << message[i] << " ," << endl;
        }
    }



    

}


