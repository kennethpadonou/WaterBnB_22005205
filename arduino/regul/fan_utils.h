#ifndef FAN_UTILS_H
#define FAN_UTILS_H

#include <Arduino.h>
extern int numberKeyPresses;
void IRAM_ATTR isr();
extern bool fanActive;
void setupFan();
void controlFan(float temperature);

#endif
