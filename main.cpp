#include <Arduino.h>
#include "comm_bt.h"
#include "controls.h"
#include "debug.h"
#include "motor.h"
#include "pins.h"
#include "sensors.h"
#include "test_controller.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    initMotor();
    initBt();
    initControls();
	initSensors();
}

void loop() {
	handleControls();
    handleEfficiencyTest();
    handleDebug();
    delay(50);
}