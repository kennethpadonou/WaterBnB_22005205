#include "json_utils.h"
#include "temperature_utils.h"
#include "led_utils.h"
#include "light_utils.h"
#include "fan_utils.h"
#include "fire_utils.h"

#include "OneWire.h"
#include "DallasTemperature.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <WiFi.h>

String jsonData;
String target_IP = "127.0.0.1";
int target_port = 1880;
int target_status = 2;
String ssid_target = ""; //"iPhone du K";
String password_target = "";//"00000000";
String macAddress = "B0-DC-EF-C4-66-B0";
String ipAdress = "172.19.150.137";
int statusESP = 2;
String location = "Les lucioles";
String ssid = "iPhoneK";
String password = "00000000";
String longitude = "7.017369"; //"7.050628185";
String latitude = "43.552847"; //"43.62453842";
bool hostpost = true;
bool occuped = false;
int seuilLumiere = 220;

void affichedataJson() {
  getLatitudeLongitude();
  DynamicJsonDocument doc(1024);

  // Collect the temperature and light data
  float temperature = getTemperature();
  int lightIntensity = analogRead(lightSensorPin);

  // Status
  doc["status"]["temperature"] = temperature;
  doc["status"]["light"] = lightIntensity;
  doc["status"]["regul"] = "HALT"; 
  //doc["status"]["fire"] = lightIntensity > seuilIncendie;
  doc["status"]["fire"] = getFireAlert();
  doc["status"]["heat"] = (temperature < thresholdLow) ? "ON" : "OFF";
  doc["status"]["cold"] = (temperature > thresholdHigh) ? "ON" : "OFF";
  doc["status"]["fanspeed"] = (temperature > thresholdHigh) ? fanSpeed : 0; //edit : 255 : 0; // assuming fan speed is 0-255
  
  doc["piscine"]["hotspot"] = hostpost;
  doc["piscine"]["occuped"] = occuped;
  // Location
  doc["location"]["room"] = "312"; 
  doc["location"]["gps"]["lat"] = latitude; //43.62453842; 
  doc["location"]["gps"]["lon"] = longitude;//7.050628185;
  doc["location"]["address"] = "Les lucioles"; 

  
  // Regul
  doc["regul"]["lt"] = thresholdLow;
  doc["regul"]["ht"] = thresholdHigh;

  // Info
  doc["info"]["ident"] = "P_2200205"; 
  doc["info"]["user"] = "Kenneth Padonou"; 
  doc["info"]["loc"] = "A Sophia"; 

  // Net
  doc["net"]["uptime"] = getUptime();
  doc["net"]["ssid"] = getSSID();
  doc["net"]["mac"] = getMacAddress();
  doc["net"]["ip"] = getIPAddress();
  doc["net"]["target_ssid"] = ssid_target.c_str() ;
  doc["net"]["target_password"] = password_target.c_str()  ;
  

  // Reporthost
  doc["reporthost"]["target_ip"] = getTargetIP();
  doc["reporthost"]["target_port"] = getTargetPort();
  doc["reporthost"]["sp"] = getStatusCode();
}




// Fonction pour calculer la distance en kilomètres entre deux points géographiques
double calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  // Convertir les degrés en radians
  float lat1Rad = radians(lat1);
  float lon1Rad = radians(lon1);
  float lat2Rad = radians(lat2);
  float lon2Rad = radians(lon2);

  // Rayon de la Terre en kilomètres
  const float earthRadiusKm = 6371.0;

  // Calculer la différence de latitudes et de longitudes
  float dLat = lat2Rad - lat1Rad;
  float dLon = lon2Rad - lon1Rad;

  // Appliquer la formule de Haversine
  float a = pow(sin(dLat / 2), 2) + cos(lat1Rad) * cos(lat2Rad) * pow(sin(dLon / 2), 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double distance = earthRadiusKm * c;

  return distance;
}

// Fonction pour vérifier si la distance entre vous et la position actuelle est inférieure ou égale à 10 km
bool isWithin10Km(float myLat, float myLon, float currentLat, float currentLon) {
  // Calculer la distance entre vous et la position actuelle
  double distance = calculateDistance(myLat, myLon, currentLat, currentLon);

  // Vérifier si la distance est inférieure ou égale à 10 km
  if (distance <= 10.0) {
    return true;
  } else {
    return false;
  }
}




void extractValuesFromJson(String message) {
  // Créer un objet DynamicJsonDocument et le désérialiser à partir du message JSON
  //getLatitudeLongitude();
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  //Serial.println("TRACE ******************************************************************* !");
  // Vérifier si le topic est TOPIC_IOT
  if (doc.containsKey("location") && doc["location"].containsKey("gps") &&
      doc["location"]["gps"].containsKey("lon") && doc["location"]["gps"].containsKey("lat") &&
      doc.containsKey("info") && doc["info"].containsKey("ident") &&
      doc.containsKey("status")) {
    // Extraire les valeurs de longitude, d'identifiant et de latitude
    float longitude_sub = doc["location"]["gps"]["lon"];
    float latitude_sub = doc["location"]["gps"]["lat"];
    String identifiant = doc["info"]["ident"];
    Serial.print("IDENT : " );
    Serial.println(identifiant);
    float latitude_float = latitude.toFloat();
    float longitude_float = longitude.toFloat();
    //double distance = calculateDistance(latitude_sub,longitude_sub,latitude,longitude);
    bool proximite = isWithin10Km(latitude_sub,longitude_sub,latitude_float,longitude_float);
    int lightIntensity = analogRead(lightSensorPin);
      if (lightIntensity < seuilLumiere) {
        occuped = true;
        fireLedOnBoard(lightIntensity);
        Serial.println("ICI ****** !");
      }
      else {occuped = false;}

  if (identifiant != "P_2200205" && proximite) {
    Serial.println("PROOOOXIMITEEEEEEEEEE ****** !");
    // Vérifier si le status contient la section "piscine"
    if (doc["status"].containsKey("piscine")) {
      // Récupérer la valeur du dernier hotspot publié sur le topic
      bool lastHotspot = doc["status"]["piscine"]["hotspot"];

      // Si le dernier hotspot est vrai, comparer la température
      if (lastHotspot) {
        // Récupérer la température actuelle
        float currentTemperature = getTemperature();

        // Récupérer la température du dernier hotspot
        //float lastHotspotTemperature = doc["status"]["temperature"];
        float lastHotspotTemperature = doc["status"]["temperature"].as<float>();


        // Si la température actuelle est plus grande que la température du dernier hotspot,
        // se déclarer en tant que hotspot
        if (currentTemperature > lastHotspotTemperature) {
          Serial.println("Je  suis  HOTSPOT ****** !");
          //doc["status"]["piscine"]["hotspot"] = true;
          hostpost = true;
        }else {hostpost = false; Serial.println("Je ne suis pas HOTSPOT ****** !");}
      }

      // Calculer la valeur de "occuped" en fonction de la lumière détectée
      //occuped = false;
      /*
      int lightIntensity = analogRead(lightSensorPin);
      if (lightIntensity < seuilLumiere) {
        occuped = true;
        fireLedOnBoard(lightIntensity);
      }
      else {occuped = false;}
      */
      // Modifier le JSON pour ajouter les champs "hotspot" et "occuped"
      //doc["status"]["piscine"]["occuped"] = occuped;
    } else {
      Serial.println("Erreur : La section 'piscine' est manquante dans le JSON.");
    }
    }

    // Utiliser les valeurs extraites comme nécessaire
    /*
    Serial.print("Longitude : ");
    Serial.println(longitude);
    Serial.print("Latitude : ");
    Serial.println(latitude);
    Serial.print("Identifiant : ");
    Serial.println(identifiant);
    */
  } else {
    Serial.println("Erreur : Certaines valeurs manquent dans le JSON.");
  }
  /*
  Serial.print("hostpost: ");
Serial.println(hostpost);
Serial.print("occuped: ");
Serial.println(occuped);
*/
}




DynamicJsonDocument sendJson() {
  //getLatitudeLongitude();
  DynamicJsonDocument doc(1024);

  // Collect the temperature and light data
  float temperature = getTemperature();
  int lightIntensity = analogRead(lightSensorPin);

  // Status
  doc["status"]["temperature"] = temperature;
  doc["status"]["light"] = lightIntensity;
  doc["status"]["regul"] = "HALT"; 
  //doc["status"]["fire"] = lightIntensity > seuilIncendie;
  doc["status"]["fire"] = getFireAlert();
  doc["status"]["heat"] = (temperature < thresholdLow) ? "ON" : "OFF";
  doc["status"]["cold"] = (temperature > thresholdHigh) ? "ON" : "OFF";
  doc["status"]["fanspeed"] = (temperature > thresholdHigh) ? fanSpeed : 0; //edit : 255 : 0; // assuming fan speed is 0-255
  doc["piscine"]["hotspot"] = hostpost;
  doc["piscine"]["occuped"] = occuped;
  // Location
  doc["location"]["room"] = "312"; 
  doc["location"]["gps"]["lat"] = latitude; //43.62453842; 
  doc["location"]["gps"]["lon"] = longitude;//7.050628185;
  doc["location"]["address"] = "Les lucioles"; 

  
  // Regul
  doc["regul"]["lt"] = thresholdLow;
  doc["regul"]["ht"] = thresholdHigh;

  // Info
  doc["info"]["ident"] = "P_22005205", //"P_2200205"; 
  doc["info"]["user"] = "Kenneth Padonou"; 
  doc["info"]["loc"] = "A Sophia"; 

  // Net
  doc["net"]["uptime"] = getUptime();
  doc["net"]["ssid"] = getSSID();
  doc["net"]["mac"] = getMacAddress();
  doc["net"]["ip"] = getIPAddress();
  doc["net"]["target_ssid"] = ssid_target.c_str() ;
  doc["net"]["target_password"] = password_target.c_str()  ;
  

  // Reporthost
  doc["reporthost"]["target_ip"] = getTargetIP();
  doc["reporthost"]["target_port"] = getTargetPort();
  doc["reporthost"]["sp"] = getStatusCode();
  
  // Serialize JSON to serial
  //Serial.println("Affichage du JSON :");
  //serializeJson(doc, Serial);
  return doc;
  //Serial.println("\nFin de l'affichage du JSON");
  //Serial.println(); 
}


void checkSerialData() {
  if (Serial.available() > 0)
  {
    jsonData = Serial.readStringUntil('\n'); // Lire la chaîne JSON jusqu'à la nouvelle ligne
    updateFromJson(jsonData);                       // Mettre à jour les configurations à partir de JSON
  }
}
// Cette fonction met à jour les seuils et les configurations WiFi
void updateFromJson(const String& jsonData) {
  StaticJsonDocument<512> doc; // Taille à ajuster en fonction des besoins
  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
    Serial.print(F("deserializeJson() a échoué avec le code "));
    Serial.println(error.c_str());
    return;
  }

  // Mise à jour des seuils de température
  if (doc.containsKey("thresholdHigh")) {
    //thresholdHigh = doc["thresholdHigh"].as<float>();
  }

  if (doc.containsKey("thresholdLow")) {
    //thresholdLow = doc["thresholdLow"].as<float>();
  }

  // Mise à jour des informations WiFi
  if (doc.containsKey("ssid")) {
    ssid_target = doc["ssid"].as<String>();
  }
  if (doc.containsKey("password")) {
    password_target = doc["password"].as<String>();
  }
  //AJOUT
  if (doc.containsKey("target_ip")) {
    setTargetIP(doc["target_ip"].as<String>());
  }

  if (doc.containsKey("target_port")) {
    setTargetPort(doc["target_port"].as<int>());
  }

  if (doc.containsKey("target_status")) {
    setStatusCode(doc["target_status"].as<int>());
  }

  //END AJOUT
  if (!ssid_target.isEmpty() && !password_target.isEmpty()) {
  connectWiFi_param(ssid_target.c_str(), password_target.c_str());
}
  if (doc.containsKey("ssid") && doc.containsKey("password")) {
    
    ssid_target = doc["ssid"].as<String>();
    password_target = doc["password"].as<String>();
    connectWiFi_param(ssid_target.c_str(), password_target.c_str());
  }

}









/***** GETTER INFORMATION PC *******/

// Get the SSID of the WiFi network
String getSSID() {
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.SSID();
  } else {
    return ssid;//"eduroam";
  }
}
void setSSID(const String& ssid_value) {
  ssid = ssid_value;
}

String getSSIDTarget() {
  return ssid_target;
}

void setSSIDTarget(const String& ssid_tar) {
  ssid_target = ssid_tar;
}

String getPasswordTarget() {
  return password_target;
}

void setPasswordTarget(const String& password) {
  password_target = password;
}
String getPassword() {
  return password;
}

void setPassword(const String& password_value) {
  password = password_value;
}

bool getFireAlert() {
 /* if (WiFi.status() == WL_CONNECTED) {
    return WiFi.SSID();
  } else {*/
    return fireDetected;
  //}
}


String getMacAddress() {
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.macAddress();
  } else {
    return macAddress; // "B0-DC-EF-C4-66-B0";
  }
}


String getIPAddress() {
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress ip = WiFi.localIP();
    return ip.toString();
  } else {
    return ipAdress; //"172.19.150.137";
  }
}


unsigned long getUptime() {
  return millis() / 1000;
}


String getTargetIP() {
  return target_IP;//"127.0.0.1"; 
}

void setTargetIP(const String& ip) {
  target_IP = ip;
}

int getTargetPort() {
  return target_port; //1880; 
}
void setTargetPort(int port) {
  target_port = port;
}

int getStatusCode() {
  return target_status; //2; 
}
void setStatusCode(int status) {
  target_status = status;
}

 int getStatusESP() {
  return statusESP;
}

void setStatusESP(int status) {
  statusESP = status;
}

String getLocation() {
  return location;
}

void setLocation(const String& loc) {
  location = loc;
}

void getLatitudeLongitude() {
  HTTPClient http;

  // Send the request to ipinfo.io to get geolocation data
  http.begin("http://ipinfo.io/json");
  int httpResponseCode = http.GET();

  // If the request was successful
  if (httpResponseCode == HTTP_CODE_OK) {
    // Parse the JSON response
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, http.getString());

    // Extract latitude and longitude from the JSON response
    String loc = jsonDoc["loc"]; // "lat,lon"
    int commaIndex = loc.indexOf(',');
    latitude = loc.substring(0, commaIndex);
    longitude = loc.substring(commaIndex + 1);

    // Print latitude and longitude
    Serial.print("Latitude: ");
    Serial.println(latitude);
    Serial.print("Longitude: ");
    Serial.println(longitude);
  } else {
    Serial.print("Error getting geolocation data. HTTP code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}