#include "SPIFFS.h"
 
void testUploadFiles() {
  const char* filesToUpload[] = {"/example1.txt", "/example2.txt"}; // Add more files as needed
  int numFiles = 2;
  uploadFiles(filesToUpload, numFiles);
}

void uploadFiles(const char** fileNames, int numFiles) {
  Serial.println("Uploading files to SPIFFS:");

  if (!SPIFFS.begin(true)) {// "true" enable format if mount fails (only for ESP32)
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  for (int i = 0; i < numFiles; ++i) {
    const char* fileName = fileNames[i];
    Serial.print("Uploading file: ");
    Serial.println(fileName);

    // Open file on SPIFFS
    File file = SPIFFS.open(fileName, "r");
    if (!file) {
      Serial.println("Failed to open file for reading");
      continue;
    }

    // Create file on SPIFFS to write
    File destFile = SPIFFS.open(fileName, "w");
    if (!destFile) {
      Serial.println("Failed to open file for writing");
      continue;
    }

    // Copy contents from source to destination
    while (file.available()) {
      destFile.write(file.read());
    }

    // Close files
    file.close();
    destFile.close();
    Serial.println("File uploaded successfully.");
  }
}