#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_TEST_RUN_NUMBER_ADDR          0

void initEeprom();

int16_t readInt16FromEeprom(int addr);
void writeInt16ToEeprom(int addr, int16_t value);

int32_t readInt32FromEeprom(int addr);
void writeInt32ToEeprom(int addr, int32_t value);
