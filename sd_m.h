#include <SD.h>

void initSd();
void writeToFileOnSdCard(char* fileName, char *data);

void mountCard();
void unmountCard();
int writeToFile(char* fileName, char *data);