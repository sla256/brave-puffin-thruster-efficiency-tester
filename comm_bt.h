#include <Stream.h>

void initBt();
Stream* getBtSerial();
bool isBtOn();
bool isBtConnected();
void btPrintln(char *s);
void btPrintln(int i);