#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <Arduino.h>

// Déclarations des fonctions
void affichedataJson();
void updateFromJson(const String& jsonData);
String getSSID();
void setSSID(const String& ssid_value);
String getSSIDTarget();
void setSSIDTarget(const String& ssid_tar);
String getPasswordTarget();
void setPasswordTarget(const String& password);
String getPassword();
void setPassword(const String& password_value);
bool getFireAlert();
String getMacAddress();
String getIPAddress();
unsigned long getUptime();
String getTargetIP();
void setTargetIP(const String& ip);
int getTargetPort();
void setTargetPort(int port);
int getStatusCode();
int getStatusESP();
void setStatusESP(int status);
void setStatusCode(int status);
String getLocation();
void setLocation(const String& loc);

// Déclarations des variables externes
extern String jsonData;
extern String target_IP;
extern int target_port;
extern int target_status;
extern String ssid_target;
extern String password_target;
extern String macAddress;
extern String ipAddress;
extern int statusESP;
extern String location;
extern String ssid;
extern String password;
extern String longitude;
extern String latitude;

#endif

