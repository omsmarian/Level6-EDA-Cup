#ifndef MQTTListener2_H
#define MQTTListener2_H

#include "MQTTClient2.h"
#include <iostream>

using namespace std;

class MQTTListener2: public MQTTListener
{
public:
    void onMessage(std::string topic, std::vector<char> payload);
    vector<float> message;
};



#endif