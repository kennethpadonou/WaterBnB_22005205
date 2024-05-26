#include "temperature_utils.h"
#include "led_utils.h"

Adafruit_NeoPixel strip(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

void setupLED() {
    pinMode(onboardLEDPin, OUTPUT);   // Configurer la LED onboard en sortie
    
    strip.begin();
    delay(1);
    for (int i = 0; i < 1; i++) {
        //turn color to red
        strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    for (int i = 1; i < 4; i++) {
        //turn color to green
        strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    for (int i = 4; i < NUMLEDS; i++) {
        //turn color to blue
        strip.setPixelColor(i, strip.Color(0, 0, 255));
    }
    strip.show();
}

void displayLEDStatus(float temperature) {
    if (temperature > thresholdHigh) {
        // Afficher ROUGE
        for (int i = 0; i < NUMLEDS; i++) {
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        }
        strip.show();
    } else if (temperature < thresholdLow) {
        // Afficher VERT
        for (int i = 0; i < NUMLEDS; i++) {
            strip.setPixelColor(i, strip.Color(0, 255, 0));
        }
        strip.show();
    } else {
        // Afficher BLEU
        for (int i = 0; i < NUMLEDS; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 255));
        }
        strip.show();
    }
}

void setLedGreen() {
    for (int i = 0; i < NUMLEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    strip.show();
}
void setLedRed() {
    for (int i = 0; i < NUMLEDS; i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    strip.show();
}
void setLedYellow() {
    for (int i = 0; i < NUMLEDS; i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 0));
    }
    strip.show();
}
//Si le feu
void fireLedOnBoard(int lightIntensity) {
  if (lightIntensity < 20) { // Ajout de la condition pour allumer la LED si l'intensité est inférieure à 20
    digitalWrite(onboardLEDPin, HIGH);
  }
  else {
    digitalWrite(onboardLEDPin, LOW);
  }
}

//}
