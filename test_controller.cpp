#include <Arduino.h>
#include "comm_bt.h"
#include "eeprom_m.h"
#include "sd_m.h"
#include "motor.h"
#include "sensors.h"
#include "test_controller.h"

const int testStepDurationMs = 5*1000;
char buffer2[100];

int16_t currentTestRun = 0; // to print for
int currentTestStep = 0; // 1-100 during each test run
int stepToStopTestRunAt = 100;
unsigned long currentTestStepStartTimeMs;

long currTestStepCumulativeCurrentValuesMa;
long currTestStepCumulativeVoltageValuesMv;
float currTestStepCumulativePowerValuesW; // all power values measured in the current test step, added up
long currTestStepCumulativeThrustForceValuesG; // all pull force values measured in the current test step, added up
int numOfMeasurementsInCurrTestStep; // number of samples, to calculate average power value at the end of the step

int motorPulseWidthValuesForAllTestSteps[100];
int averageCurrentValuesForAllTestSteps[100]; // in mA
int averageVoltageValuesForAllTestSteps[100]; // in mV
float averagePowerValuesForAllTestSteps[100]; // in watts
int averageThrustForceValuesForAllTestSteps[100]; // in grams

char* printFormatMeasuredData(int step);

void progressToNextStep() {
    currentTestStep++;
    currentTestStepStartTimeMs = millis();
    currTestStepCumulativeCurrentValuesMa = 0;
    currTestStepCumulativeVoltageValuesMv = 0;
    currTestStepCumulativePowerValuesW = 0;
    currTestStepCumulativeThrustForceValuesG = 0;
    numOfMeasurementsInCurrTestStep = 0;
    
    // ramp up the motor but don't measure yet - will be done later in handleCurrentTest; throttle can go to 101 briefly
    setMotorThrottle(currentTestStep);
}

void beginNewEfficiencyTest(int testStepToStopAt=100) {
    currentTestRun = readInt16FromEeprom(EEPROM_TEST_RUN_NUMBER_ADDR) + 1;
    writeInt16ToEeprom(EEPROM_TEST_RUN_NUMBER_ADDR, currentTestRun);

    stepToStopTestRunAt = testStepToStopAt;

    Serial.print("Beginning test run #");
    Serial.println(currentTestRun);
    btPrintln(currentTestRun);
    Serial.print("Stopping at ");
    Serial.println(testStepToStopAt);

    currentTestStep = 0;
    progressToNextStep();
}

void handleTestInProgress() {
    if (currentTestStep == 0) {
        return;
    }

    if (millis() - currentTestStepStartTimeMs > testStepDurationMs) {
        // print latest values from the previous step
        char* formattedData = printFormatMeasuredData(currentTestStep - 1);
        Serial.println(btPrintln(formattedData));
        progressToNextStep();
        return;
    }

    if (currentTestStep > stepToStopTestRunAt) {
        stopEfficiencyTest();
        return;
    }

    // still in the middle of an active test step, measure and record power & thrust force
    int currentMa = getCurrentFromIna226();
    int voltageMv =  getVoltageFromIna226();
    float powerW = currentMa * voltageMv / 1000000;

    // calculate averages
    currTestStepCumulativeCurrentValuesMa += currentMa;
    currTestStepCumulativeVoltageValuesMv += voltageMv;
    currTestStepCumulativePowerValuesW += powerW;
    currTestStepCumulativeThrustForceValuesG += getPullForce();
    numOfMeasurementsInCurrTestStep++;

    int i = currentTestStep - 1;
    averageCurrentValuesForAllTestSteps[i] = currTestStepCumulativeCurrentValuesMa / numOfMeasurementsInCurrTestStep;
    averageVoltageValuesForAllTestSteps[i] = currTestStepCumulativeVoltageValuesMv / numOfMeasurementsInCurrTestStep;
    averagePowerValuesForAllTestSteps[i] = currTestStepCumulativePowerValuesW / numOfMeasurementsInCurrTestStep;
    averageThrustForceValuesForAllTestSteps[i] = currTestStepCumulativeThrustForceValuesG / numOfMeasurementsInCurrTestStep;
    motorPulseWidthValuesForAllTestSteps[i] = getCurrentMotorPulseWidth();
}

void stopEfficiencyTest() {
    currentTestStep = 0;
    stopMotor();

    Serial.print("Finished test run #");
    Serial.println(currentTestRun);
    btPrintln(currentTestRun);

    char testResultsFileName[20];
    snprintf(testResultsFileName, sizeof(testResultsFileName), "/r%d_%d.csv", currentTestRun, millis() / 1000);
    writeToFileOnSdCard(testResultsFileName, "Step, Pulse width, Current mA, Voltage mV, Power watts, Force grams");

    for(int i = 0; i < stepToStopTestRunAt; i++) {
        char* formattedData = printFormatMeasuredData(i);
        writeToFileOnSdCard(testResultsFileName, formattedData);
        Serial.println(btPrintln(formattedData));
    }
}

char* printFormatMeasuredData(int step) {
    snprintf(buffer2, sizeof(buffer2), "%d, %d, %d, %d, %.1f, %d", 
        step + 1,
        motorPulseWidthValuesForAllTestSteps[step],
        averageCurrentValuesForAllTestSteps[step],
        averageVoltageValuesForAllTestSteps[step],
        averagePowerValuesForAllTestSteps[step],
        averageThrustForceValuesForAllTestSteps[step]);

    return buffer2;
}