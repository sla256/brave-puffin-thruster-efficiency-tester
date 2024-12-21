#include "debug.h"
#include <ESP32Servo.h>
#include "motor.h"
#include "pins.h"

// 40A ESC
// Inits at 1500
// Backward 1443 (min) to 1150 (max)
// Forward 1505 (min) to 1920 (max)

// 30A ESC
// Inits at 800
// Forward 1190 (min) to 1890 (max)
const int motorStopPulseWidth = 800;    // must start at this level to initialize ESC properly
const int motorMinPulseWidth = 1190;
const int motorMaxPulseWidth = 1890;

Servo motorControl;

int currentMotorPulseWidth = motorStopPulseWidth;
int currentMotorThrottleLevel = 0;

void initMotor() {
    Serial.println("MOTOR");
    pinMode(PIN_MOTOR_PWM, OUTPUT);
    motorControl.attach(PIN_MOTOR_PWM);
    stopMotor();
}

void stopMotor() {
    currentMotorPulseWidth = motorStopPulseWidth;
    currentMotorThrottleLevel = 0;
    motorControl.write(currentMotorPulseWidth);
}

void setMotorThrottle(int throttleLevel) {
    currentMotorThrottleLevel = throttleLevel;
    currentMotorPulseWidth = map(throttleLevel, 1, 100, motorMinPulseWidth, motorMaxPulseWidth);
    motorControl.write(currentMotorPulseWidth);
}

bool incrementMotorThrottle(int throttleLevelIncrement) {
    setMotorThrottle(currentMotorThrottleLevel + throttleLevelIncrement);
    return true;
}

int getCurrentMotorPulseWidth() {
    return currentMotorPulseWidth;
}