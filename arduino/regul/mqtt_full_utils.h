#ifndef MGTT_FULL_UTILS_H
#define MGTT_FULL_UTILS_H


void setupMqtt();
void set_LED(int v);
float get_Temperature();
void mqtt_pubcallback(char* topic, 
                      byte* payload, 
                      unsigned int length);
void mqtt_subscribe_mytopics();
void loopMQTT();

#endif
