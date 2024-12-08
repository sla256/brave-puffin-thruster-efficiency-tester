#include <Arduino.h>

#include "debug.h"
#include "pins.h"
#include "sd_m.h"

bool sdInitStatus;

// char fileName[13];
char buffer[50];

void initSd() {
  Serial.print("SD card: ");
  mountCard();
  Serial.println(sdInitStatus ? "OK" : "FAIL");

//   writeInt16ToEeprom(EEPROM_LOG_FILE_NUMBER_ADDR, 0);
  unmountCard();

  snprintf(buffer, sizeof(buffer), "SD init at %d", millis());
  Serial.println(buffer);
  writeToFileOnSdCard("/init.txt", buffer);
}

void writeToFileOnSdCard(char* fileName, char *data) {
  mountCard();
  if (!sdInitStatus) {
    Serial.println("SD write aborted");
    unmountCard();
    return; 
  }

//   snprintf(fileName, 12, "/%d-%d.csv", currentRunNumber(), logFileNumber);
  
  int writtenBytes = writeToFile(fileName, data);
  if (writtenBytes <= 0) {
    Serial.println("SD write failed");
  }

  unmountCard();
}

void mountCard() {
  // safe to call repeatedly if card is already mounted
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/src/SD.cpp
  sdInitStatus = SD.begin(PIN_SD_CS);
  if (!sdInitStatus) {
    Serial.println("Failed to mount SD card");
  }
}

void unmountCard() {
  // safe to call repeatedly if card is not mounted
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/src/SD.cpp
  
  // however memory will leak if called repeatedly to mount / unmount, until this is fixed:
  // https://github.com/espressif/arduino-esp32/issues/2897
  SD.end(); 
}

int writeToFile(char* fileName, char *data) {
  File file = SD.open(fileName, FILE_APPEND);
  if (!file) {
    return 0;
  } 
  
//   int writtenBytes = file.print(currentRunNumber());
//   writtenBytes += file.print(",");
  
//   writtenBytes += file.print(logFileNumber);
//   writtenBytes += file.print(",");

//   writtenBytes += file.print(logRecordCount);
//   writtenBytes += file.print(",");
  
  int writtenBytes = file.println(data);
  file.close();

  return writtenBytes;
}
