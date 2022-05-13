#include "MQTTListener2.h"

void MQTTListener2::onMessage(std::string topic, std::vector<char> payload)
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


