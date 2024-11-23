#include <Arduino.h>
#include "comm_bt.h"
#include "eeprom_m.h"
#include "sd_m.h"
#include "motor.h"
#include "sensors.h"
#include "test_controller.h"

const int testStepDurationMs = 3*1000;
char buffer2[50];

int16_t currentTestRun = 0; // to print for
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
    currentTestRun = readInt16FromEeprom(EEPROM_TEST_RUN_NUMBER_ADDR) + 1;
    writeInt16ToEeprom(EEPROM_TEST_RUN_NUMBER_ADDR, currentTestRun);

    testStepToStopTestRunAt = testStepToStopAt;

    Serial.print(btPrintln("Beginning test run #"));
    Serial.print(currentTestRun);
    Serial.print(", stopping at ");
    Serial.println(testStepToStopAt);

    currentTestStep = 0;
    progressToNextStep();
}

void handleTestInProgress() {
    if (currentTestStep == 0) {
        stopMotor();
        return;
    }

    if (millis() - currentTestStepStartTimeMs > testStepDurationMs) {
        // print latest values from the previous step
        snprintf(buffer2, sizeof(buffer2), "%d: %.1f, %d", 
            currentTestStep,
            averagePowerValuesForAllTestSteps[currentTestStep - 1], 
            averageThrustForceValuesForAllTestSteps[currentTestStep - 1]);
        Serial.println(btPrintln(buffer2));

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

    Serial.print(btPrintln("Finished test run #"));
    Serial.println(btPrintln(currentTestRun));

    char testResultsFileName[20];
    snprintf(testResultsFileName, sizeof(testResultsFileName), "/r%d_%d.csv", currentTestRun, millis() / 1000);
    writeToFileOnSdCard(testResultsFileName, "Power in watts, force in grams");

    for(int i = 0; i < testStepToStopTestRunAt; i++) {
        snprintf(buffer2, sizeof(buffer2), "%.1f, %d", averagePowerValuesForAllTestSteps[i], averageThrustForceValuesForAllTestSteps[i]);
        writeToFileOnSdCard(testResultsFileName, buffer2);
        Serial.println(btPrintln(buffer2));
    }
}