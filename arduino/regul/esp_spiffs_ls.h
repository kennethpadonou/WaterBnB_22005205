#ifdef ESP_SPIFFS_LS_H

  #define ESP_SPIFFS_LS_H
  #include "SPIFFS.h"

  void testUploadFiles();

  void uploadFiles(const char** fileNames, int numFiles);

#endif // ESP_SPIFFS_LS_H