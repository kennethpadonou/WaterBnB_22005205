#include "http_as_serverasync_routes.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"


#include "json_utils.h"
#include "light_utils.h"
#include "temperature_utils.h"
#include "fan_utils.h"

#define USE_SERIAL Serial

/*===================================================*/
String processor(const String& var){
  if(var == "TEMPERATURE"){
    return String(getTemperature());
  }
  else if(var == "LIGHT"){
    return String(readLightIntensity());
  }
  else if(var == "SSID"){
    return getSSID();
  }
  else if(var == "MAC"){
    return getMacAddress();
  }
  else if(var == "IP"){
    return getIPAddress();
  }
  else if(var == "WHERE"){
    return "Les lucioles";
  }
  else if(var == "LT"){
    return String(thresholdLow);
  }
  else if(var == "HT"){
    return String(thresholdHigh);
  }
  else if(var == "PRT_IP"){
    return getTargetIP();
  }
  else if(var == "PRT_PORT"){
    return String(getTargetPort());
  }
  else if(var == "PRT_T"){
    return String(getStatusCode());
  }
  else if(var == "COOLER"){
    //return myEspModel.coolerState ? "ON" : "OFF";
  }
  else if(var == "HEATER"){
    //return myEspModel.heaterState ? "ON" : "OFF";
  }
  else if(var == "UPTIME"){
    //return String(String((millis() - myEspModel.startTimer) / 1000));
  }
  else if (var == "REGUL") {
    //return myEspModel.regulationState ? "RUNNING" : "HALT";
  }
  else if (var == "FIRE") {
    return fireDetected ? "DETECTED" : "NOT DETECTED";
  }
  else if (var == "FANSPEED") {
    //return String(myEspModel.fanSpeed);
    return fanActive ? "HIGH" : "LOW";
  }
  else if (var == "LONG") {
    //return String(myEspModel.longitude);
    return "7.0";
  }
  else if (var == "LAT") {
    //return String(myEspModel.latitude);
    return "7.0";
  }
  else if (var == "ROOM") {
    //return String(myEspModel.room);
    return "Salle 1";
  }
  else if (var == "IDENT") {
    return "ESP32 123";
  }
  else if (var == "USER") {
    return "GM";
  }
  else if (var == "LOC") {
    return "A Biot";
  }

  return String(); // Parce que c'est requis par la documentation de AsyncWebServer
}


/*===================================================*/
void setup_http_routes(AsyncWebServer* server) {
  float temperature = getTemperature();


  server->serveStatic("/", SPIFFS, "/").setTemplateProcessor(processor);  
  
  // Declaring root handler, and action to be taken when root is requested
  auto root_handler = server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });

  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/target", [](AsyncWebServerRequest *request, JsonVariant &json) {
      Serial.println("Requete reçue pour un rapport périodique !");

      // Créer un objet JSON à partir de la valeur JSONVariant
      JsonObject jsonObj = json.as<JsonObject>();
      //String target_ip = jsonObj["ip"];
      target_IP = jsonObj["ip"].as<String>();
      target_port = jsonObj["port"].as<int>();
      target_status = jsonObj["sp"].as<int>();
      String error = "";

      if (!target_IP || !target_port || !target_status) {
        request->send(400, "text/plain", "Missing parameters.");
        return;
      }

      if (target_port > 0 && target_port < 65535) {
        //myEspModel.target_port = target_port;
        setTargetPort(target_port); // Utilisation de la variable externe
      } else {error += "Invalid port.";}

      if (target_status > 0) {
        //myEspModel.target_sp = target_sp;
        setStatusCode(target_status); 
      } else {error += "Invalid sampling period.";}
      //myEspModel.target_ip = target_ip;

      if (error == "") {
        request->redirect("/");
      } else {
        request->redirect("/?error=" + error);
      }
      // Traiter les données JSON comme vous le souhaitez
  });

  server->addHandler(handler);


  server->on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
      /* Une route qui renvoie la valeur de la lumière */
      request->send(200, "text/plain", String(readLightIntensity()));
  });

  server->on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
      /* Une route qui renvoie la valeur de la température */
      request->send(200, "text/plain", String(getTemperature()));
  });

  server->on("/value", HTTP_GET, [temperature](AsyncWebServerRequest *request){ //server->on("/value", HTTP_GET, [temperature](AsyncWebServerRequest *request){
     Serial.println("Requête reçue pour les valeurs !");

      DynamicJsonDocument jsonDoc(256);

      for (size_t i = 0; i < request->params(); i++) {
        String paramName = request->getParam(i)->name();
        if (paramName == "temperature") {
          jsonDoc["temperature"] = getTemperature();
        } else if (paramName == "light") {
          jsonDoc["light"] = readLightIntensity();
        } else if (paramName == "regul") {
          //jsonDoc["regul"] = myEspModel.regulationState ? "RUNNING" : "HALT";
          jsonDoc["regul"] = "Running";
        } else if (paramName == "fire") {
          jsonDoc["fire"] = fireDetected ? "DETECTED" : "NOT DETECTED";
        } else if (paramName == "heat") {
          jsonDoc["heat"] = (temperature < thresholdLow) ? "ON" : "OFF";
        } else if (paramName == "cold") {
          jsonDoc["cold"] = (temperature > thresholdHigh) ? "ON" : "OFF";
        } else if (paramName == "fanspeed") {
          jsonDoc["fanspeed"] =  (temperature > thresholdHigh) ? 255 : 0;
        } else {
          request->send(404, "application/json", "{\"error\": \"Paramètre inconnu : " + paramName + "\"}");
          return;
        }
      }

      String jsonResponse;
      serializeJson(jsonDoc, jsonResponse);

      request->send(200, "application/json", jsonResponse);
  });

  server->on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    /* Une route qui permet aux utilisateurs de modifier des valeurs via des paramètres GET */
    for (size_t i = 0; i < request->params(); i++) {
      String paramName = request->getParam(i)->name();
      Serial.print(paramName);
      Serial.println(" : " + request->getParam(i)->value());
      if (paramName == "regul") {
        String regulValue = request->getParam(i)->value();
        //regulationState = regulValue == "RUNNING";
      } else if (paramName == "fire") {
        String fireValue = request->getParam(i)->value();
        //fireDetected = fireValue == "DETECTED";
      } else if (paramName == "heat") {
        String heatValue = request->getParam(i)->value();
        //(temperature < thresholdLow) ? "ON" : "OFF";
        //heaterState = heatValue == "ON";
      } else if (paramName == "cold") {
        String coldValue = request->getParam(i)->value();
        //coolerState = coldValue == "ON";
      } else if (paramName == "fanspeed") {
        String fanSpeedValue = request->getParam(i)->value();
        fanSpeed = fanSpeedValue.toInt();
        //fanSpeed = fanSpeedValue.toInt();
        //ledcWrite(PWM_CHANNEL, fanSpeed);
      } else if (paramName == "lt") {
        String ltValue = request->getParam(i)->value();
        thresholdLow = ltValue.toFloat(); //VERIF

        //lowThreshold = ltValue.toFloat();
      } else if (paramName == "ht") {
        String htValue = request->getParam(i)->value();
        thresholdHigh = htValue.toFloat();
      } else {
        request->send(404, "text/plain", "Paramètre inconnu : " + paramName);
        return;
      }
    }
    request->send(200, "text/plain", "Paramètres mis à jour !");
  });

  server->addHandler(handler);
  server->onNotFound([](AsyncWebServerRequest *request){
      request->send(404);
    });
}
