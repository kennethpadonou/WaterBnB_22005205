#ifndef LIGHT_UTILS_H
#define LIGHT_UTILS_H

const int onboardLEDPin = 2;   // Utilisation du numéro de broche GPIO2 pour la LED onboard
const int seuilIncendie = 500; // Seuil d'intensité lumineuse pour détecter un incendie

const int lightSensorPin = A5;      // Pin du capteur de lumière (ADC1 Channel 5 / GPIO33)

void setupLightSensor();
int readLightIntensity();

#endif
