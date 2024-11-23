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
    int currentPowerW =  voltage * current / 1000000;
    int force = getPullForce(); // g

    Serial.print("V="); Serial.print(voltage);
    Serial.print("mV; I="); Serial.print(current);
    Serial.print("mA; P="); Serial.print(currentPowerW);
    Serial.print("W; F="); Serial.print(force);
    Serial.print("g"); Serial.println();

    btPrintln("mV, mA, W, g");
    btPrintln(voltage);
    btPrintln(current);
    btPrintln(currentPowerW);
    btPrintln(force);
}

