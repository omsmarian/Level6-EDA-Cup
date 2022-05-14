#ifndef MQTTListener2_h
#define MQTTListener2_h

#include "MQTTClient2.h"

class MQTTListener2: public MQTTListener
{
public:
	MQTTListener2(MQTTClient2 *mqtt);
	void onMessage(std::string topic, std::vector<char> payload);
	

private:
	MQTTClient2* miau;
	std::vector<float> ballPos;
	std::vector<float> ballVel;
	std::vector<float> ballRot;
	std::vector<float> ballAngVel;
};




#endif // !MQTTListener2_h
