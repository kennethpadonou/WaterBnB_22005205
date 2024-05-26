#ifndef TEMPERATURE_UTILS_H
#define TEMPERATURE_UTILS_H

#include <OneWire.h>
#include <DallasTemperature.h>

extern OneWire oneWire;
extern DallasTemperature tempSensor;

extern float thresholdHigh;
extern float thresholdLow;
extern int fanSpeed;

float getTemperature();
void controlTemperature(float temperature);
void displayTemperatureStatus(float temperature);

#endif
