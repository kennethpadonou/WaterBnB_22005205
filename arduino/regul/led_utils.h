#ifndef LED_UTILS_H
#define LED_UTILS_H

#include <Adafruit_NeoPixel.h>

#define PIN 13
#define NUMLEDS 5
extern Adafruit_NeoPixel strip;

const int ledPin1 = 19;  // Broche pour la première LED (D19)
const int ledPin2 = 21;  // Broche pour la deuxième LED (D18)

void setupLED();
void displayLEDStatus(float temperature);
void fireLedOnBoard(int lightIntensity);

#endif
