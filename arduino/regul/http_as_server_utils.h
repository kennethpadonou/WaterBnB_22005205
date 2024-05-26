#ifndef WIFICLIENT_UTILS_H
#define WIFICLIENT_UTILS_H
/*
#include <WiFi.h>
#include "wifi_utils.h"

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer 
unsigned long loop_period = 1L * 1000; // =>  1000ms : 1 s //

#define USE_SERIAL Serial
*/
// Instanciation of a "Web server" on port 80 



void httpReply(WiFiClient client)
void setuphttpServer()
void loophttpServer()
#endif
