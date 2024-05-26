// #include "header_utils.h"
#include <Adafruit_NeoPixel.h>
#include <SPIFFS.h>

#include "temperature_utils.h"
#include "led_utils.h"
#include "light_utils.h"
#include "fan_utils.h"
#include "fire_utils.h"
#include "wifi_utils.h"

#include "OneWire.h"

#include "wifiClient_utils.h"
#include "http_as_server_utils.h"
#include "httpClientSecure_utils.h"
#include "httpClient_utils.h"
#include "http_as_serverasync.h"
#include "mqtt_full_utils.h"


#include "DallasTemperature.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "json_utils.h"
#include <HTTPClient.h>

unsigned long lastTime = 0;
unsigned long loop_period = 10L * 1000; /* => 10000ms : 10 s */
unsigned long startTime;
const unsigned long connectionTimeout = 10000;
const char *ssid_AP = "ManDaPad";
const char *password_AP = "00000000";


#define MODE_AP 0 // Définir MODE_AP à 0 pour désactiver le mode point d'accès

void setup()
{
  Serial.begin(9600); // starts the serial port at 9600
  SPIFFS.begin(true);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  delay(2000);
  setupWifi();
  Serial.println("Point d'accès démarré !");
  Serial.print("Adresse IP du Point d'accès : ");
  Serial.println(WiFi.softAPIP());
  tempSensor.begin(); 
  Serial.print("setupFan : ");
  setupFan();
  Serial.print("setupLed : ");
  setupLED();
  Serial.print("setupLight : ");
  setupLightSensor();

  startTime = millis(); 
  setupAsyncServer();
  setupMqtt();
  setLedGreen();
}

void loop()
{

 checkSerialData();
 loopMQTT();
  float temperature = getTemperature();
  int lightIntensity = readLightIntensity();
  fireAlert(temperature, lightIntensity);
  Serial.print("lumiere: ");
  Serial.print(lightIntensity);
  Serial.print(" \n");

  String target_address = String(target_IP.c_str()) +":"+ String("1880") + "/esp?mac=B0-DC-EF-C4-66-B0";

  String response = httpPOSTRequest(target_address.c_str(), jsonData.c_str()); 
  Serial.print("Response : "); Serial.println(response);
}

void signalerIncendie()
{
  digitalWrite(onboardLEDPin, HIGH);
  delay(5000);
  digitalWrite(onboardLEDPin, LOW);
}
