
#ifndef HTTP_ASYNC_H
#define HTTP_ASYNC_H

#include <Arduino.h>

extern String LEDState;
extern String last_temp;
extern String last_light;
//extern AsyncWebServer asyncServer;
extern short int Light_threshold;


// Autres déclarations de fonctions ou de constantes

//#endif // HTTP_ASYNC_H
/*
#include <ArduinoOTA.h>
#include "ArduinoJson.h"
#include <WiFi.h>
//#include "ESPAsyncWebServer.h"
#include "wifi_utils.h"
#include "sensors.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "SPIFFS.h"

#include "routes.h"
*/


void DoSmtg(int delai);
void httpServerAsyncsetup();
void httpServerAsyncloop();

#endif
