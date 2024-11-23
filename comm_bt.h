#include <Stream.h>

void initBt();
Stream* getBtSerial();
bool isBtOn();
bool isBtConnected();
char* btPrintln(char *s);
int btPrintln(int i);