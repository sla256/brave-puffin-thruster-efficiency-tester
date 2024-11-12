#include <ESP32Servo.h>
#include "motor.h"
#include "pins.h"

const int motorStopPulseWidth = 800;    // must start at this level to initialize ESC properly
const int motorMinPulseWidth = 1200;
const int motorMaxPulseWidth = 1600;

Servo motorControl;

int currentMotorPulseWidth = motorStopPulseWidth;

void initMotor() {
    Serial.println("MOTOR");
    pinMode(PIN_MOTOR_PWM, OUTPUT);
    motorControl.attach(PIN_MOTOR_PWM);
    stopMotor();
}

void stopMotor() {
    currentMotorPulseWidth = motorStopPulseWidth;
    motorControl.write(currentMotorPulseWidth);
}

void setMotorThrottle(int throttleLevel) {
    currentMotorPulseWidth = map(throttleLevel, 1, 100, motorMinPulseWidth, motorMaxPulseWidth);
    motorControl.write(currentMotorPulseWidth);
}

bool incrementMotorThrottle(int throttleLevelIncrement) {
    if (currentMotorPulseWidth + throttleLevelIncrement > motorMaxPulseWidth) return false;

    setMotorThrottle(currentMotorPulseWidth + throttleLevelIncrement);
    return true;
}