
/*============================================*/
WiFiServer server(80);

void httpReply(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after 
                                        //completion of the response  
  client.println("Refresh: 5");         // refresh the page automatically every 5 sec
  
  client.println(); // EMPTY LINE between header and body
  
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");    
  client.print("Hello, je tourne depuis : "); // Returns the ms passed since the ESP 
                                              // began running the current program. 
  client.print(millis()/1000); // On pourrait sans doute donner une info
  client.println("s <br />");  // plus pertinente ? temperature ? 
  client.println("</html>"); 
}

/*============================================*/
/*---- Arduino IDE paradigm : setup+loop -----*/
/*============================================*/
void setuphttpServer()//void setup()
{ 
  /* Serial connection -----------------------*/
  USE_SERIAL.begin(9600);
  while(!USE_SERIAL); //wait for a serial connection  
  /* WiFi connection  -----------------------*/
  String hostname = "Mon petit objet ESP32";
  wifi_connect_multi(hostname);               
  /* WiFi status     --------------------------*/
  if (WiFi.status() == WL_CONNECTED){
    USE_SERIAL.print("\nWiFi connected : yes ! \n"); 
    wifi_printstatus(0);  
  } 
  else {
    USE_SERIAL.print("\nWiFi connected : no ! \n"); 
    //  ESP.restart();
  }
  server.begin(); 
}

void loophttpServer()// void loop() 
{  /* Requests processing */
  WiFiClient client = server.available(); // listen for incoming clients
  if (client) { // incoming client
    USE_SERIAL.println("New client is connecting !");

    boolean currentLineIsBlank = true; // an http request ends with a blank line CRLF
    while (client.connected()) {      
      if (client.available()) {
        char c = client.read(); 
        USE_SERIAL.write(c); // ECHO the request (char by char) on the console
	
        if (c == '\n' && currentLineIsBlank) {  // if you've gotten to a CRLF the http GET request has ended,
          httpReply(client);   // so you can send the REPLTY
          break;
        }
        if (c == '\r') { // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') { // you've gotten a character on the current line
          currentLineIsBlank = false; // this is not a blank line
        }
      } // end if
    }// end while
    
    // give the web browser time to receive the data
    delay(loop_period); // ms
    client.stop();
    USE_SERIAL.println("client disconnected");
  }
}
