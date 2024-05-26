#include "temperature_utils.h"
#include "led_utils.h"

bool fireDetected = false;
void fireAlert(float temperature, int lightIntensity) {
  if (lightIntensity < 20 & temperature>thresholdHigh) { // Ajout de la condition pour allumer la LED si l'intensité est inférieure à 20
    digitalWrite(onboardLEDPin, HIGH);
    fireDetected = true;
  }
  else {
    digitalWrite(onboardLEDPin, LOW);
    fireDetected = false;
  }
}

//}
