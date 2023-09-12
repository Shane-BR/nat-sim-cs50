#ifndef HELPERS
#define HELPERS

#include "datatypes.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

void delay(int milliseconds);
int randomInt(int min, int max);
double randomDouble();
int powerLawRandomInt(int min, int max, double tail_index);
int strToInt(char* str);
bool compareString(char* str1, char* str2); // Returns false if strings do not match
bool comparePosition(position pos1, position pos2);
int distanceBetweenPositions(position pos1, position pos2);
int8_t getNationIndex(char* nation_name);
char* getNationName(int8_t index);
bool runProbability(float percent_chance);
bool inMapBounds(position pos);
int tileHash(position pos);
double clamp(double val, double min, double max);
double logistic(double x, double L, double k, double x0);

#endif