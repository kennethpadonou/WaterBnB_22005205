#include "fan_utils.h"

const int fanPin = 26; // Exemple de broche utilisée pour le ventilateur
//bool fanSpeed = 0; // Vitesse du ventilateur (0-255)
bool fanActive = false; // Vitesse du ventilateur (0-255)
int numberKeyPresses = 0;
void IRAM_ATTR isr() { // Interrupt Handler
  numberKeyPresses++;
}

void setupFan() {
    pinMode(26, INPUT_PULLUP); // Configuration de la broche du ventilateur en entrée avec pull-up interne
    attachInterrupt(26, isr, FALLING); // Attache de l'interruption sur la broche du ventilateur
    ledcAttachPin(27, 0); // Attache du canal PWM à la broche du ventilateur
    ledcSetup(0, 25000, 8); // Configuration du canal PWM avec une fréquence de 25 kHz et une résolution de 8 bits
    ledcAttachPin(27, 0);
    ledcWrite(0, 255); // Démarrage du ventilateur à pleine vitesse

}

void controlFan(float temperature) {
    if (temperature > thresholdHigh) {
        // Si la température dépasse le seuil élevé, activer le ventilateur
        digitalWrite(fanPin, HIGH);
        fanActive = true;
    } else {
        // Sinon, désactiver le ventilateur
        digitalWrite(fanPin, LOW);
        fanActive = false;
    }
}
