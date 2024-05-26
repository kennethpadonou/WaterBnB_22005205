#include "httpClient_utils.h"


#include <WiFi.h>
#include "wifi_utils.h"
#include <HTTPClient.h>
#define USE_SERIAL Serial
/*============================================*/
String httpClientGETRequest(const char *UrlServer) //String httpGETRequest(const char *UrlServer)
{

    String payload = "{}";
    HTTPClient http; // http protocol entity => client

    USE_SERIAL.printf("Requesting URL : %s\n", UrlServer);
    http.begin(UrlServer);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0)
    {
        USE_SERIAL.printf("HTTP Response code : %d", httpResponseCode);
        payload = http.getString();
    }
    else
    {
        USE_SERIAL.printf("Error code on HTTP GET Request : %d", httpResponseCode);
    }
    http.end();

    return payload;
}

void setuphttpClient()//void setup()
{
    /* Serial connection -----------------------*/
    USE_SERIAL.begin(9600);
    while (!USE_SERIAL)
        ; // wait for a serial connection

    String hostname = "Mon petit objet ESP32";
    wifi_connect_multi(hostname);

    /* WiFi status --------------------------*/
    if (WiFi.status() == WL_CONNECTED)
    {
        USE_SERIAL.print("\nWiFi connected : yes ! \n");
        wifi_printstatus(0);
    }
    else
    {
        USE_SERIAL.print("\nWiFi connected : no ! \n");
        // ESP.restart();
    }
}

void loophttpClient() //void loop()
{
    char host[100] = "http://httpbin.org"; // = "http://worldtimeapi.org/";
    const int httpPort = 80;

    // Make Url
    String path = "/get";
    String params = "?led1="
                    "OFF"
                    "&led2="
                    "ON"
                    "";
    String url = String(host) + path + params;

    if ((millis() - lastTime) > loop_period)
    {
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {

            String ret = httpClientGETRequest(url.c_str());//httpGETRequest(url.c_str());
            USE_SERIAL.println(ret);
        }
        else
        {
            USE_SERIAL.println("WiFi Disconnected");
        }
        lastTime = millis();
    }
}

String httpPOSTRequest(const char* UrlServer, const char* payload) {
  String response = "{}";
  HTTPClient http;

  USE_SERIAL.printf("Sending POST request to URL : %s\n", UrlServer);

  http.begin(UrlServer);

  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    USE_SERIAL.printf("HTTP Response code : %d\n", httpResponseCode);
    response = http.getString();
  } else {
    USE_SERIAL.printf("Error code on HTTP POST Request : %d\n", httpResponseCode);
  }

  http.end();

  return response;
}