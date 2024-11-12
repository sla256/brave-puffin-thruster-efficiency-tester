#include <Arduino.h>
#include <BluetoothSerial.h>
#include "comm_bt.h"

BluetoothSerial SerialBT;

bool btOnOffState;

void turnBtOn();

void initBt() {
  Serial.print("BT: ");

  Serial.println(SerialBT.begin("bptet-esp32", false, false));

  turnBtOn();
}

Stream* getBtSerial() {
  return &SerialBT;
}

bool isBtOn() {
  return btOnOffState  &&  isBtConnected();
}

void turnBtOn() {
  btStart();
  SerialBT.enableSSP();
  btOnOffState = true;
}

bool isBtConnected() {
  return SerialBT.connected();
}

void btPrintln(char *s) {
  if (!isBtOn() ||  !isBtConnected()) {
    return;
  }
  getBtSerial()->println(s);
}

void btPrintln(int i) {
  if (!isBtOn()  ||  !isBtConnected()) {
    return;
  }
  getBtSerial()->println(i);
}
