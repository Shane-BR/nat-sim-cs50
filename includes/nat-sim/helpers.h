#ifndef NS_HELPERS
#define NS_HELPERS

#include "datatypes.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

int randomInt(int min, int max);
double randomDouble();
int powerLawRandomInt(int min, int max, double tail_index);
int strToInt(char* str);
bool compareString(const char* str1, const char* str2); // Returns false if strings do not match
bool comparePosition(position pos1, position pos2);
int distanceBetweenPositions(position pos1, position pos2);
int8_t getNationIndex(char* nation_name);
char* getNationName(int8_t index);
nation* getNation(int8_t index);
bool runProbability(float percent_chance);
void addToDynamicPointerArray(void*** arr, unsigned int* size, void* add, unsigned int* mem_block_size);
bool removeFromDynamicPointerArray(void*** arr, unsigned int* size, void* remove, bool free_data);
bool removeFromDynamicPointerArrayAtIndex(void*** arr, unsigned int* size, int removeIndex, bool free_data);
void eraseDynamicPointerArray(void*** arr, unsigned int* size, bool free_data);
int tileHash(position pos);
double clamp(double val, double min, double max);
double logistic(double x, double L, double k, double x0);

#endif