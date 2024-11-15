#include <Arduino.h>
#include "comm_bt.h"
#include "motor.h"
#include "sensors.h"
#include "test_controller.h"

const int testStepDurationMs = 3*1000;

int currentTestRun = 0; // to print for
int currentTestStep = 0; // 1-100 during each test run
int testStepToStopTestRunAt = 100;
unsigned long currentTestStepStartTimeMs;

float currentTestStepCumulativePowerValuesW; // all power values measured in the current test step, added up
long currentTestStepCumulativeThrustForceValuesG; // all pull force values measured in the current test step, added up
int numberOfMeasurementsInCurrentTestStep; // number of samples, to calculate average power value at the end of the step

float averagePowerValuesForAllTestSteps[100]; // in watts
int averageThrustForceValuesForAllTestSteps[100]; // in grams

void progressToNextStep() {
    currentTestStep++;
    currentTestStepStartTimeMs = millis();
    currentTestStepCumulativePowerValuesW = 0;
    currentTestStepCumulativeThrustForceValuesG = 0;
    numberOfMeasurementsInCurrentTestStep = 0;
    
    // ramp up the motor but don't measure yet - will be done later in handleCurrentTest; throttle can go to 101 briefly
    setMotorThrottle(currentTestStep);
}

void beginNewEfficiencyTest(int testStepToStopAt=100) {
    currentTestRun++;
    testStepToStopTestRunAt = testStepToStopAt;

    Serial.print("Beginning test run #");
    Serial.print(currentTestRun);
    Serial.print(", stopping at ");
    Serial.println(testStepToStopAt);

    currentTestStep = 0;
    progressToNextStep();
}

void handleEfficiencyTest() {
    if (currentTestStep == 0) {
        stopMotor();
        return;
    }

    if (millis() - currentTestStepStartTimeMs > testStepDurationMs) {
        // print latest values from the previous step
        Serial.print(currentTestStep);
        Serial.print(": ");
        Serial.print(averagePowerValuesForAllTestSteps[currentTestStep - 1], 1);
        Serial.print(", ");
        Serial.println(averageThrustForceValuesForAllTestSteps[currentTestStep - 1]);

        btPrintln(currentTestStep);

        progressToNextStep();
        return;
    }

    if (currentTestStep > testStepToStopTestRunAt) {
        stopEfficiencyTest();
        return;
    }

    // still in the middle of an active test step, measure and record power & thrust force
    float currentPowerW = getCurrentFromIna226() * getVoltageFromIna226() / 1000000;
    currentTestStepCumulativePowerValuesW += currentPowerW;
    currentTestStepCumulativeThrustForceValuesG += getPullForce();
    numberOfMeasurementsInCurrentTestStep++;

    averagePowerValuesForAllTestSteps[currentTestStep - 1] = 
        currentTestStepCumulativePowerValuesW / numberOfMeasurementsInCurrentTestStep;
    averageThrustForceValuesForAllTestSteps[currentTestStep - 1] = 
        currentTestStepCumulativeThrustForceValuesG / numberOfMeasurementsInCurrentTestStep;
}

void stopEfficiencyTest() {
    currentTestStep = 0;
    stopMotor();

    Serial.print("Finished test run #");
    Serial.println(currentTestRun);

    for(int i = 0; i < testStepToStopTestRunAt; i++) {
        Serial.print(averagePowerValuesForAllTestSteps[i], 1);
        Serial.print(", ");
        Serial.println(averageThrustForceValuesForAllTestSteps[i]);
    }
}