void initSensors();
void scanI2cDevices();

int getCurrentFromIna226(); // in mA
int getVoltageFromIna226(); // in mV

void calibrateForceSensorHx711();
int getPullForce(); // in grams