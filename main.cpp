#include <Arduino.h>
#include "comm_bt.h"
#include "controls.h"
#include "debug.h"
#include "eeprom_m.h"
#include "motor.h"
#include "pins.h"
#include "sd_m.h"
#include "sensors.h"
#include "test_controller.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    initMotor();
    initBt();
    initControls();
	initSensors();
    initSd();
    initEeprom();

    Serial.println("Ready");
}

void loop() {
	handleControls();
    handleTestInProgress();
    handleDebug();
    delay(50);
}