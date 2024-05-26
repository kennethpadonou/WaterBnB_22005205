#ifndef HTTP_AS_SERVER_ASYNC_ROUTES_H

  #include "ESPAsyncWebServer.h"
  void setup_http_routes(AsyncWebServer* server);
  String processor(const String& var);

#endif