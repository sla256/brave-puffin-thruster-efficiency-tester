#include <Arduino.h>
#include <BluetoothSerial.h>
#include "comm_bt.h"

BluetoothSerial SerialBT;

bool btOnOffState;

void turnBtOn();

void initBt() {
  Serial.print("BT: ");

  Serial.println(SerialBT.begin("bptet-esp32", false, true));

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

char *btPrintln(char *s) {
  if (!isBtOn() ||  !isBtConnected()) {
    return s;
  }

  getBtSerial()->println(s);
  return s;
}

int btPrintln(int i) {
  if (!isBtOn()  ||  !isBtConnected()) {
    return i;
  }

  getBtSerial()->println(i);
  return i;
}
