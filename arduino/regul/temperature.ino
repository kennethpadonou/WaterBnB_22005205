#include "temperature_utils.h"
#include "led_utils.h" // Inclusion des autres fichiers .h au besoin
#include "light_utils.h"
#include "fan_utils.h"

OneWire oneWire(23);  // Définition de l'objet OneWire avec le port 23
DallasTemperature tempSensor(&oneWire);  
float thresholdHigh = 24.0;
float thresholdLow = 23.0;

int fanSpeed = 0;

float getTemperature() {
  float t;
  tempSensor.requestTemperaturesByIndex(0); 
  t = tempSensor.getTempCByIndex(0); 

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" C\n");
  delay(1000); 
  return t;
}


// Définition de la fonction controlTemperature
void controlTemperature(float temperature) {
    if (temperature > thresholdHigh) {
        Serial.println("chaud thresholdHigh"); 
        // Climatiseur ON
        digitalWrite(ledPin1, HIGH);
        digitalWrite(ledPin2, LOW);
        // Ventilateur progressif
        fanSpeed = map(temperature, thresholdHigh, 40, 0, 255);
        fanSpeed = constrain(fanSpeed, 0, 255);
        //analogWrite(27, fanSpeed);
        analogWrite(27, 255);
        // Radiateur OFF
        strip.setPixelColor(0, strip.Color(255, 0, 0));
        strip.show();
    } else if (temperature < thresholdLow) {
        Serial.println("froid thresholdLow"); 
        // Radiateur ON
        digitalWrite(ledPin1, LOW);
        digitalWrite(ledPin2, HIGH);
        // Ventilateur OFF
        analogWrite(27, 0);
        // Affichage LED
        strip.setPixelColor(0, strip.Color(0, 255, 0));
        strip.show();
    } else {
        Serial.println("Neutre"); 
        // Tout éteint
        digitalWrite(ledPin1, LOW);
        digitalWrite(ledPin2, LOW);

        // Ventilateur OFF
        analogWrite(27, LOW);

        // Affichage LED
        strip.setPixelColor(0, strip.Color(255, 165, 0)); // Orange
        strip.show();
    }
}

// Définition de la fonction displayTemperatureStatus
void displayTemperatureStatus(float temperature) {
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
        // Afficher ORANGE
        for (int i = 0; i < NUMLEDS; i++) {
            strip.setPixelColor(i, strip.Color(255, 165, 0));
        }
        strip.show();
    }
}
