#include "helpers.h"
#include "constants.h"

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern nation nations[NAT_AMOUNT];

void delay(int milliseconds)
{
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time has passed
    while (clock() < start_time + milliseconds)
        ;
}

// Generates a random 32-bit integer from MIN to MAX (inclusive).
int randomInt(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

// Generates a random double percision floating-point number between 0 to 1.
double randomDouble()
{
    return (double)rand() / RAND_MAX;
}

/**
 * powerLawRandomInt: Generates a random integer from a power law distribution.
 *
 * @param min The minimum value of the output range, inclusive. Must be positive.
 * @param max: The maximum value of the output range, inclusive. Must be greater than min.
 * @param tail_index: The exponent parameter of the power law distribution. Must be positive.
 *               A larger tail index means a faster decay of the tail and a more uniform distribution.
 *               A smaller tail index means a slower decay of the tail and a more skewed distribution.
 * @returns
 *   A random integer between min and max, drawn from a power law distribution with the given tail index.
 *   If any of the parameters are invalid, returns -1 and prints an error message.
 */
int powerLawRandomInt(int min, int max, double tail_index)
{
    return min + (max - min) * pow(randomDouble(), tail_index);
}

// Returns 0 if no integer can be returned
int strToInt(char* str)
{
    int val = 0;
    bool is_neg = false;

    if (*str == '-') { is_neg = true; str++; }
    if (*str == '+') { is_neg = false; str++; }

    for (int i = 0; str[i] != 0; i++) 
    {   
        char c = str[i];
        
        if (c > 57 || c < 48)
            break;

        if (val > INT_MAX/10)
            return 0;

        val = val*10 + (c - 48);
    }
    return is_neg ? val*-1 : val;
}

bool compareString(char* str1, char* str2)
{
    if (str1 == NULL || str2 == NULL)
        return false;

    for (int i = 0; str1[i] != 0; i++)
    {
        if (str2[i] == 0)
            return false;

        if (str1[i] != str2[i])
            return false;
    }

    return true;
}

bool comparePosition(position pos1, position pos2)
{
    return pos1.x == pos2.x && pos1.y == pos2.y;
}


int distanceBetweenPositions(position pos1, position pos2)
{
    int xdif = pos1.x - pos2.x;
    int ydif = pos1.y - pos2.y;
    int root = sqrt(xdif*xdif + ydif*ydif);

    return root;
}

// Returns -1 if index not found
int8_t getNationIndex(char* nation_name)
{
    for (int8_t i = 0; i < NAT_AMOUNT; i++)
    {
        if(compareString(nations[i].name, nation_name))
        {
            return i;
        }
    }

    return -1;
}

char* getNationName(int8_t index)
{
    return nations[index].name;
}

// Randomly returns true given the probability.
bool runProbability(float percent_chance)
{   
    if ((percent_chance) <= 0)
        return false;

    if (percent_chance >= 100)
        return true;

    float rand_num = randomDouble()*100;

    return rand_num <= percent_chance;
}

bool inMapBounds(position pos)
{
    return pos.y >= 0 && pos.y <= MAP_SIZE && pos.x >= 0 && pos.x <= MAP_SIZE;
}

// Returns a unique hash code for every possible position on the map
int tileHash(position pos)
{
    return pos.x + pos.y * MAP_SIZE;
}

double clamp(double val, double min, double max)
{
    return val > max ? max : val < min ? min : val;
}

/**
 * Calculates the value of the logistic function for a given set of parameters.
 *
 * @param x The value of the independent variable.
 * @param L The maximum value of the function.
 * @param k The logistic growth rate or steepness of the curve.
 * @param x0 The value of the function's midpoint.
 * @return The value of the logistic function for the given set of parameters.
 */
double logistic(double x, double L, double k, double x0)
{
    return L / (1 + exp(-k * (x - x0)));
}