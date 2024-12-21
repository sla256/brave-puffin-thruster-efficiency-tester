#include <Arduino.h>
#include "comm_bt.h"
#include "motor.h"
#include "pins.h"
#include "sensors.h"
#include "test_controller.h"

bool isInDebugMode = false;

void handleSerialInputs();
void handleBtInputs();
void processControlInput(char input);

void initControls() {
    pinMode(PIN_STATUS_LED, OUTPUT);
}

void handleControls() {    
    handleSerialInputs();
    handleBtInputs();

    digitalWrite(PIN_STATUS_LED, isInDebugMode ? HIGH : LOW);
}

void handleSerialInputs() {
    if (Serial.available() <= 0) return;
    
    processControlInput((char) Serial.read());
}

void handleBtInputs() {
    if (!isBtOn()  ||  getBtSerial() == NULL  ||  !isBtConnected()) return;
    if (getBtSerial()->available() <= 0) return;

    processControlInput((char) getBtSerial()->read());
}

void processControlInput(char input) {
    btPrintln(input);

    switch (input) {
        case 'a':
            isInDebugMode = !isInDebugMode;
            break;

        case 'B':
            beginNewEfficiencyTest(100);
            break;

        case 'b':
            beginNewEfficiencyTest(50);
            break;

        case 'c':
            calibrateForceSensorHx711();
            break;

        case 's':
            isInDebugMode = false;
            stopMotor();
            stopEfficiencyTest();
            break;
            
        case 'g':
            setMotorThrottle(1);
            Serial.println(btPrintln(getCurrentMotorPulseWidth()));
            break;

        case 'u':
            incrementMotorThrottle(1);
            Serial.println(btPrintln(getCurrentMotorPulseWidth()));
            break;

        case 'd':
            incrementMotorThrottle(-1);
            Serial.println(btPrintln(getCurrentMotorPulseWidth()));
            break;
    }
}

bool getDebugModeState () {
    return isInDebugMode;
}