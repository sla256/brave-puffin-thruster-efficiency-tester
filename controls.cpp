#include <Arduino.h>
#include "comm_bt.h"
#include "motor.h"
#include "pins.h"
#include "sensors.h"
#include "test_controller.h"

void handleSerialInputs();
void handleBtInputs();
void processControlInput(char input);

void initControls() {
    pinMode(PIN_STATUS_LED, OUTPUT);
}

void handleControls() {
    handleSerialInputs();
    handleBtInputs();
}

void handleSerialInputs() {
    if (Serial.available() <= 0) return;
    
    digitalWrite(PIN_STATUS_LED, HIGH);
    processControlInput((char) Serial.read());
    digitalWrite(PIN_STATUS_LED, LOW);
}

void handleBtInputs() {
    if (!isBtOn()  ||  getBtSerial() == NULL  ||  !isBtConnected()) return;
    if (getBtSerial()->available() <= 0) return;

    processControlInput((char) getBtSerial()->read());
}

void processControlInput(char input) {
    unsigned long currentPowerW;

    switch (input) {
        case 'a':
            currentPowerW = getVoltageFromIna226() * getCurrentFromIna226() / 1000000;
            Serial.println(getVoltageFromIna226());
            Serial.println(getCurrentFromIna226());
            Serial.println(currentPowerW);

            btPrintln(getVoltageFromIna226());
            delay(100);
            btPrintln(getCurrentFromIna226());
            delay(100);
            btPrintln(currentPowerW);
            break;

        case 'b':
            beginNewTest();
            break;

        case 's':
            stopMotor();
            stopCurrentTest();
            break;
            
        case 'g':
            setMotorThrottle(1);
            break;

        case 'u':
            incrementMotorThrottle(10);
            break;
            
        case 'd':
            incrementMotorThrottle(-10);
            break;
    }
}