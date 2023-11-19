#ifndef SCREEN
#define SCREEN

#include "datatypes.h"

void initScreen();

void drawChars(char* str, int x, int y, int max_length, bool overflow, int layer);
void drawSingleChar(char c, int x, int y, int layer);
const int getScreenSizeX();
const int getScreenSizeY();
int highestEmptyScreenLayer(int x, int y);
void drawScreen(void);

#endif