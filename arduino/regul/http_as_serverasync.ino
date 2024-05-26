#include "esp_spiffs_ls.h"

#include "http_as_serverasync_routes.h"


// Create AsyncWebServer object on port 80
AsyncWebServer async_server(80);

void setupAsyncServer(){
  setup_http_routes(&async_server);
  async_server.begin();
}
