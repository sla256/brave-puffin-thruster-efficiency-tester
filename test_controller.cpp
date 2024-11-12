#include <Arduino.h>
#include "comm_bt.h"
#include "motor.h"
#include "sensors.h"
#include "test_controller.h"

const int testStepDurationMs = 1*1000;

int currentTestStep = 0; // 1-100 during test
unsigned long currentTestStepStartTimeMs;

float currentTestStepCumulativePowerValuesW; // all values of power measured in the current test step, added up
int numberOfPowerMeasurementsInCurrentTestStep; // number of samples, to calculate average power value at the end of the step

float averagePowerValuesForAllTestSteps[100];

// TODO track thrust force
void progressToNextStep() {
    currentTestStep++;
    currentTestStepStartTimeMs = millis();
    currentTestStepCumulativePowerValuesW = 0;
    numberOfPowerMeasurementsInCurrentTestStep = 0;
    
    // ramp up the motor but don't measure yet - will be done later in handleCurrentTest; throttle can go to 101 briefly
    setMotorThrottle(currentTestStep);
}

void beginNewEfficiencyTest() {
    currentTestStep = 0;
    progressToNextStep();
}

void handleEfficiencyTest() {
    if (currentTestStep == 0) {
        stopMotor();
        return;
    }

    if (millis() - currentTestStepStartTimeMs > testStepDurationMs) {
        // output latest values from the previous step
        Serial.print(currentTestStep);
        Serial.print(": ");
        Serial.println(averagePowerValuesForAllTestSteps[currentTestStep - 1], 1);

        btPrintln(currentTestStep);
        delay(50);
        btPrintln(averagePowerValuesForAllTestSteps[currentTestStep - 1]);

        progressToNextStep();
        return;
    }

    if (currentTestStep > 100) {
        stopEfficiencyTest();
        return;
    }

    // in the middle of an active test step, measure and record power
    float currentPowerW = getCurrentFromIna226() * getVoltageFromIna226() / 1000000;
    currentTestStepCumulativePowerValuesW += currentPowerW;
    numberOfPowerMeasurementsInCurrentTestStep++;
    averagePowerValuesForAllTestSteps[currentTestStep - 1] = currentTestStepCumulativePowerValuesW / numberOfPowerMeasurementsInCurrentTestStep;
}

void stopEfficiencyTest() {
    // TODO write results to an SD card
    currentTestStep = 0;
    stopMotor();

    for(int i = 0; i < 100; i++) {
        Serial.print(averagePowerValuesForAllTestSteps[i], 1);
        Serial.print(", ");
    }

    Serial.println();
}