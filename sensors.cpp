#include <HX711.h>
#include <INA226.h>
#include "sensors.h"
#include "pins.h"

#define INA226_I2C_ADDRESS1 0x40
INA226 ina226_1(INA226_I2C_ADDRESS1);

HX711 forceSensor;

void initSensors() {
  Wire.begin();
  scanI2cDevices();

  Serial.print("INA226_1: ");
  Serial.println(ina226_1.begin());
  ina226_1.setMaxCurrentShunt(20, 0.002);
}

int getCurrentFromIna226() {
  return ina226_1.getCurrent_mA();
}

int getVoltageFromIna226() {
  return ina226_1.getBusVoltage_mV();
}

void scanI2cDevices() {
  Serial.println("I2C scan: ");

  for(uint8_t address = 1; address < 127; address++)   {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    Serial.print(".");
 
    if (error != 0) continue;

    Serial.print("I2C found: 0x");
    if (address < 16) { Serial.print("0"); }
    Serial.println(address, HEX);
  }

  Serial.println();
}