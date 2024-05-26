#include "wifiClient_utils.h"
#include <WiFi.h>
#include "wifi_utils.h"

void httpGETRequest(const char *httpServer, const int httpPort = 80)
{
   WiFiClient client; // https://www.arduino.cc/reference/en/libraries/wifi/
    USE_SERIAL.printf("=================================\n");
    if (client.connect(httpServer, httpPort) == 0)
    {
        USE_SERIAL.printf("Requesting URL : %s => failed !\n", httpServer);
        return;
    }
    else
    {
        USE_SERIAL.printf("URL %s connected !\n", httpServer);
    }
    String url = "/ip";
    String req = String("GET "); // Now create HTTP request header INCLUDING URL
    req += url + " HTTP/1.1\r\n";
    req += "host: " + String(httpServer) + "\r\n";
    req += "User-Agent: esp-idf/1.0 esp32 \r\n";
    req += "Accept: */* \r\n";
    req += "Connection: close \r\n";
    req += "\r\n"; 

    USE_SERIAL.printf("----------------------------------------\n");
    USE_SERIAL.printf("Request : \n %s", req.c_str());
    USE_SERIAL.printf("----------------------------------------\n");
    client.print(req); // Send !
    unsigned long timeout = millis();
    while (client.available() == 0)
    { 
        if (millis() - timeout > 10000)
        {
            USE_SERIAL.println(">>> Client Timeout ! => Disconnection !");
            client.stop(); // disconnect
            return;
        }
    }
    USE_SERIAL.println("Response : ");
    while (client.available())
    { 
        String line = client.readStringUntil('\r');
        USE_SERIAL.print(line); // echo to console "this" line

    }
    USE_SERIAL.println("Closing connection from client side");
    client.stop(); 
}

void setupWifiClient() 
{
    USE_SERIAL.begin(9600);
    while (!USE_SERIAL)
    String hostname = "iPhoneK";
    wifi_connect_multi(hostname);
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

void loopWifiClient() //void loop()
{
    const char httpServer[100] = "httpbin.org"; //"http://worldtimeapi.org";
    const int httpPort = 80;

    httpGETRequest(httpServer, httpPort);

    delay(10000); // ms
}
