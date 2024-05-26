#include "light_utils.h"

void setupLightSensor() {
    pinMode(lightSensorPin, INPUT);   // Configurer le capteur de lumière en entrée
    
}

int readLightIntensity() {
    return analogRead(lightSensorPin);
}
