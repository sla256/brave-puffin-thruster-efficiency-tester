#include <HX711.h>
#include <INA226.h>
#include "sensors.h"
#include "pins.h"

#define INA226_I2C_ADDRESS1 0x40
INA226 ina226_1(INA226_I2C_ADDRESS1);

HX711 forceSensor;
int hx711CalibrationOffset = 145779;
int hx711CalibrationScale = 103.414162;

// mechanical advantage of a lever pushing on the force sensor
// 2.0 means thruster could be product 1 kg of force and the sensor registering 2 kg
const float rigLeverRatio = 3.6;

void initSensors() {
  // pinMode(RX, INPUT);

  Wire.begin();
  scanI2cDevices();

  Serial.print("INA226_1: ");
  Serial.println(ina226_1.begin() ? "OK" : "FAIL");
  ina226_1.setMaxCurrentShunt(20, 0.002);

  Serial.print("HX711: ");
  forceSensor.begin(PIN_HX711_DATA, PIN_HX711_CLOCK);
  forceSensor.set_offset(hx711CalibrationOffset);
  Serial.println(forceSensor.set_scale(hx711CalibrationScale) ? "OK" : "FAIL");
  forceSensor.tare();
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

void calibrateForceSensorHx711() {
  Serial.println("Ensure no pull force; enter any character to proceed");
  while (!Serial.available());

  forceSensor.tare(20);
  hx711CalibrationOffset = forceSensor.get_offset();

  Serial.print("Offset: ");
  Serial.println(hx711CalibrationOffset);
  Serial.println("Pull with 500g force; enter any character to proceed");

  while (Serial.available()) Serial.read();
  while (!Serial.available());

  forceSensor.calibrate_scale(500, 20);
  hx711CalibrationScale = forceSensor.get_scale();

  Serial.print("Scale: ");
  Serial.println(hx711CalibrationScale);
}

int getPullForce() {
  return forceSensor.get_units() / rigLeverRatio;
}