#include <Arduino.h>
#include "comm_bt.h"
#include "controls.h"
#include "debug.h"
#include "sensors.h"

unsigned long loopRunCount = 0;

void handleDebug() {
    loopRunCount++;

    if (getDebugModeState() && loopRunCount % 10 == 0) {
        printDebugInfo();
    }
}

void printDebugInfo() {
    int voltage = getVoltageFromIna226(); // mV
    int current = getCurrentFromIna226(); // mA
    int power =  voltage * current / 1000000; // W
    int force = getPullForce(); // g

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "V=%d mV, I=%d mA, P=%d W, F=%d g", 
        voltage, current, power, force);

    Serial.println(btPrintln(buffer));
}

