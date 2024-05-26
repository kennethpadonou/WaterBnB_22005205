#include "httpClient_utils.h"

/*
2 * Fichier : https_by_HTTPClient/https_by_HTTPClient.ino
3 * Auteur : G.Menez
4
5 => based on Rui Santos work https://randomnerdtutorials.com/esp32-http-get-post-arduino/
6 based on https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/examples/BasicHttpCli7 */
const static char *ca PROGMEM =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
    "DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n"
    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n"
    "AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n"
    "ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n"
    "wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n"
    "LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n"
    "4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n"
    "bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n"
    "sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n"
    "Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n"
    "FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n"
    "SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n"
    "PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n"
    "TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n"
    "SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n"

    "c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n"
    "+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n"
    "ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n"
    "b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n"
    "U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n"
    "MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n"
    "5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n"
    "9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n"
    "WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n"
    "he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n"
    "Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n"
    "-----END CERTIFICATE-----\n";

/*============================================*/
String httpsGETRequest(const char *UrlServer, const char *ca)
{
    /*
     * This method sends data via HTTP GET requests to host/url
     * and return the response/payload of the GET request to UrlServer
     */

    String payload = "{}";
    HTTPClient http; // http protocol entity => client

    USE_SERIAL.printf("Requesting URL : %s\n", UrlServer);
    http.begin(UrlServer, ca);
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
    // End connection and Free resources
    http.end();

    return payload;
}
/*============================================*/
/*---- Arduino IDE paradigm : setup+loop -----*/
/*============================================*/
void setupthttpsClient() //void setup()
{

    USE_SERIAL.begin(9600);
    while (!USE_SERIAL)
        ; // wait for a serial connection

    /* WiFi connection -----------------------*/
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
    /* Install OTA --------------------------*/
    // setupOTA("GM_ESP");

    // your code
}

void loopthttpsClient() //void loop()
{ // cf https://www.howsmyssl.com/s/api.html
    char host[100] = "https://www.howsmyssl.com";
    const int httpPort = 443;
    // Make Url
    String path = "/a/check";
    String params = "";
    String url = String(host) + path + params;
    // Send an HTTP request every loop_period in ms
    if ((millis() - lastTime) > loop_period)
    {
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {

            String ret = httpsGETRequest(url.c_str(), ca);
            USE_SERIAL.println(ret);
        }
        else
        {
            USE_SERIAL.println("WiFi Disconnected");
        }
        lastTime = millis();
    }
}
