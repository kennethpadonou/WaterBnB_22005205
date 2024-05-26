#include "http_as_serverasync_utils.h"
#include "temperature_utils.h"
#include "light_utils.h"
#include "routes.h"

/*
 * Auteur : G.Menez
 * Fichier : http_as_serverasync/http_as_serverasync.ino

 * Many sources :
 => https://raw.githubusercontent.com/RuiSantosdotme/ESP32-Course/master/code/WiFi_Web_Server_DHT/WiFi_Web_Server_DHT.ino
 => https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
 => Kevin Levy
*/
/*====== Import required libraries ===========*/
/*
#include <ArduinoOTA.h>
#include "ArduinoJson.h"
#include <WiFi.h>

#include "wifi_utils.h"
#include "sensors.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "SPIFFS.h"

#include "routes.h"
*/
#include "ESPAsyncWebServer.h"
//#define USE_SERIAL Serial

//void setup_OTA(); // from ota.ino

/*===== ESP GPIO configuration ==============*/
/* ---- LED         ----*/
//const int LEDpin = 19; // LED will use GPIO pin 19
/* ---- Light       ----*/
//const int LightPin = A5; // Read analog input on ADC1_CHANNEL_5 (GPIO 33)
/* ---- Temperature ----*/
//OneWire oneWire(23);                    // Pour utiliser une entite oneWire sur le port 23
//DallasTemperature TempSensor(&oneWire); // Cette entite est utilisee par le capteur de temperature

/*====== ESP Statut =========================*/
// Ces variables permettent d'avoir une representation
// interne au programme du statut "electrique" de l'objet.
// Car on ne peut pas "interroger" une GPIO pour lui demander !
String LEDState = "off";
String last_temp;
String last_light;

/*====== Process configuration ==============*/
// Set timer
//unsigned long loop_period = 10L * 1000; /* =>  10000ms : 10 s */

// Create AsyncWebServer object on port 80
AsyncWebServer asyncServer(80); //AsyncWebServer server(80);

// Thresholds
short int Light_threshold = 250; // Less => night, more => day

// Host for periodic data report
String target_ip = "";
int target_port = 1880;
int target_sp = 0; // Remaining time before the ESP stops transmitting

/*=======================================================*/
/* Delay can be non compatible with OTA handle or with async !!!!
 *  because ESP32 pauses your program during the delay().
 *  If next OTA request is generated while Arduino is paused waiting for
 *  the delay() to pass, your program will miss that request.
 *   => Use a non-blocking method, using mills() and a delta to decide to read or not. eg */
void DoSmtg(int delai)
{
  static uint32_t tick = 0;
  if (millis() - tick < delai)
  {
    return;
  }
  else
  { /* Do stuff here every 10 second */
    USE_SERIAL.print("deja 10 secondes écoulées ! \n");
    tick = millis();
    //*last_temp = atoi(get_temperature(TempSensor).c_str());
    //last_temp = get_temperature(TempSensor);
    //last_light = get_light(LightPin);
    last_temp = getTemperature();
    last_light = readLightIntensity();
  }
}

/*============================================*/
/*---- Arduino IDE paradigm : setup+loop -----*/
/*============================================*/

void httpServerAsyncsetup() //void setup()
{
  /* Serial connection -----------------------*/
  USE_SERIAL.begin(9600);
  while (!USE_SERIAL)
    ; // wait for a serial connection

  /* WiFi connection  -----------------------*/
  String hostname = "Mon petit objet ESP32";
  wifi_connect_multi(hostname);

  /* WiFi status     --------------------------*/
  if (WiFi.status() == WL_CONNECTED)
  {
    USE_SERIAL.print("\nWiFi connected : yes ! \n");
    wifi_printstatus(0);
  }
  else
  {
    USE_SERIAL.print("\nWiFi connected : no ! \n");
    //  ESP.restart();
  }

  // Init OTA
  // setup_OTA();

  // Initialize the LED
  //setup_led(LEDpin, OUTPUT, LOW);

  // Init temperature sensor
  //TempSensor.begin();

  // Initialize SPIFFS
  //SPIFFS.begin(true);

  // Setup routes of the ESP Web server
  setup_http_routes(&asyncServer);

  // Start ESP Web server
  asyncServer.begin(); //server.begin();
  
}

void httpServerAsyncloop() //void loop()
{
  int delai = 10000;
  /* OTA*/
  // ArduinoOTA.handle();

  /* Update sensors */
  DoSmtg(delai);

   //delay(loop_period); // ms
   delay(10L * 1000);
}
