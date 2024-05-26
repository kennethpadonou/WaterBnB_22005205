/*********
	  Based on Rui Santos work : https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
	  File mqtt_full/mqtt_full.ino
	  Modified by GM

	  Test with CLI :
	  mosquitto_pub  -h test.mosquitto.org -t "uca/M1/iot/led" -m on -q 1
*********/
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "wifi_utils.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "temperature_utils.h"
#include "mqtt_full_utils.h"
#include "json_utils.h"
#include "led_utils.h"
#include "light_utils.h"




#define MQTT_HOST IPAddress(192, 168, 1, XXX)


const char* mqtt_server = "test.mosquitto.org"; // anynomous Ok in 2021
#define TOPIC_TEMP "uca/M1/iot/temp"
#define TOPIC_LED  "uca/M1/iot/led"
#define TOPIC_IOT  "uca/iot/piscine" //"uca/iot/piscine/22005205"
#define TOPIC_IOT2  "uca/iot/piscine/P_22005205"
/*===== ESP is a MQTT Client =======*/
WiFiClient espClient;           // Wifi 
PubSubClient mqttclient(espClient); // MQTT client
String hostname = "Mon petit objet ESP32";

#define USE_SERIAL Serial

/*===== Arduino IDE paradigm : setup+loop =====*/
void setupMqtt() {
  mqttclient.setServer(mqtt_server, 1883);
  mqttclient.setBufferSize(1024);

  mqttclient.setCallback(mqtt_pubcallback); 
}

/*============= TO COMPLETE ? ===================*/
void set_LED(int v){
  
}

float get_Temperature(){
  return 22.5;
}

/*============== CALLBACK ===================*/
void mqtt_pubcallback(char* topic, 
                      byte* payload, 
                      unsigned int length) {
  /* 
   * Callback when a message is published on a subscribed topic.
   */
  USE_SERIAL.print("Message arrived on topic : ");
  USE_SERIAL.println(topic);
  USE_SERIAL.print("=> ");

  // Byte list (of the payload) to String and print to Serial
  String message;
  for (int i = 0; i < length; i++) {
    //USE_SERIAL.print((char)payload[i]);
    message += (char)payload[i];
  }
  USE_SERIAL.println(message);
  
  extractValuesFromJson(message);
  if (String(topic) == TOPIC_LED) {
    USE_SERIAL.print("so ... changing output to ");
    if (message == "on") {
      USE_SERIAL.println("on");
      set_LED(HIGH);
    }
    else if (message == "off") {
      USE_SERIAL.println("off");
      set_LED(LOW);
    }
  }
  if (String(topic) == TOPIC_IOT2) {
    // Check if the JSON message contains the "occupied" parameter
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message);
    //affichage du message
    //serializeJsonPretty(doc, USE_SERIAL);
    Serial.println("TOPIC_IOT2*************************");
    bool occupied = doc["occupied"];
    String ledColor = doc["led_strip"].as<String>();


    if (ledColor == "jaune") {
      USE_SERIAL.println("Pool is occupied");
      //strip jaune
      setLedYellow();

    } else if (ledColor == "rouge") {
      USE_SERIAL.println("Accès piscine interdit"); 
      setLedRed();
      delay(30000);
      setLedGreen();

    }
     else if (ledColor == "vert") {
      USE_SERIAL.println("Piscine disponible"); 
      setLedGreen();

    }

  }
}

/*============= SUBSCRIBE to TOPICS ===================*/
void mqtt_subscribe_mytopics() {
  /*
   * Subscribe to MQTT topics 
   * There is no way on checking the subscriptions from a client. 
   * But you can also subscribe WHENEVER you connect. 
   * Then it is guaranteed that all subscriptions are existing.
   * => If the client is already connected then we have already subscribe
   * since connection and subscriptions go together 
   */
  while (!mqttclient.connected()) { // Loop until we're reconnected
    USE_SERIAL.print("Attempting MQTT connection...");
    
    // Attempt to connect => https://pubsubclient.knolleary.net/api
    
    String mqttclientId = "P_22005205"; //"ESP-PK22005205";
    String mqttId = "DeathStar-";
    const int mqttPort = 1883;
    const char* mqttUser = "darkvador";
    const char* mqttPassword = "6poD2R2";

    mqttclientId += getMacAddress(); //WiFi.macAddress(); // if we need random : String(random(0xffff), HEX);
    mqttclient.subscribe("uca/iot/piscine/P_22005205", 1); //mqttclient.subscribe(TOPIC_IOT2, 1);
    if (mqttclient.connect("P_22005205",
                              NULL, /* No credential */
                              NULL))  
      {
      USE_SERIAL.println("connected");
	        //uca/M1/iot
      // then Subscribe topics
      //mqttclient.subscribe(TOPIC_LED,1);
      mqttclient.subscribe(TOPIC_IOT,1);
      mqttclient.subscribe("uca/iot/piscine/P_22005205", 1); 
    } 
    else { // Connection to broker failed : retry !
      USE_SERIAL.print("failed, rc=");
      USE_SERIAL.print(mqttclient.state());
      USE_SERIAL.println(" try again in 5 seconds");
      delay(5000);
    }
  } 
}

/*================= LOOP ======================*/
void loopMQTT() { /*--- Publish Temperature periodically   */
  int32_t period = 5000; // 5 sec
  
  mqtt_subscribe_mytopics();
  delay(period);
  DynamicJsonDocument jsonDoc = sendJson();
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  char payload[100];
  sprintf(payload,"{\"temperature\" : \"%.2f\"}",getTemperature());

  mqttclient.publish(TOPIC_TEMP, payload);

  mqttclient.publish(TOPIC_IOT, jsonString.c_str());

  mqttclient.loop(); // Process MQTT event/action
}

#if 0
/*
 *  Old fashion payload and publishing
 *
 */
  char payload[100];
  char topic[150];
  // Space to store values to send
  char str_sensor[10];
  char str_lat[6];
  char str_lng[6];
  
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label
  
  float sensor = analogRead(SENSOR); 
  float lat = 6.101;
  float lng= -1.293;

  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  dtostrf(sensor, 4, 2, str_sensor);
  dtostrf(lat, 4, 2, str_lat);
  dtostrf(lng, 4, 2, str_lng);  
  
  sprintf(payload, "%s {\"value\": %s", payload, str_sensor); // Adds the value
  sprintf(payload, "%s, \"context\":{\"lat\": %s, \"lng\": %s}", payload, str_lat, str_lng); // Adds coordinates
  sprintf(payload, "%s } }", payload); // Closes the dictionary brackets
  
  client.publish(topic, payload);
#endif
