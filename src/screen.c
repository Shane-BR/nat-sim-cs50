#include <stdio.h>
#include <stdlib.h>

#include "screen.h"

#define SCREEN_SIZE_X 120
#define SCREEN_SIZE_Y 28
#define TOTAL_SCREEN_SIZE (SCREEN_SIZE_X+1) * SCREEN_SIZE_Y // +1 for newline chars
#define LAYERS 3

char screen_layers[TOTAL_SCREEN_SIZE+1][LAYERS];
char screen_final[TOTAL_SCREEN_SIZE+1];

int getScreenPos(int x, int y);
void updateFinalScreenAtIndex(int element_index);
void updateFinalScreenAtPos(int x, int y);

void initScreen()
{

    // Form grid
    for (int i = 0; i < TOTAL_SCREEN_SIZE; i++)
    {
        if ((i+1) % (SCREEN_SIZE_X+1) == 0 && i > 0) // I get why this works but... damn
        {
            screen_layers[i][0] = '\n';
        }
        else 
        {
            screen_layers[i][LAYERS-1] = '0';
        }

        updateFinalScreenAtIndex(i);

    }

    screen_layers[TOTAL_SCREEN_SIZE][0] = '\0';
}

/**
 * Draws a string of characters on the screen.
 * Will override any current characters at that position.
 *
 * @param str        - A pointer to the string of characters to be drawn.
 * @param x          - The x-coordinate of the starting point of the string.
 * @param y          - The y-coordinate of the starting point of the string.
 * @param max_length - The maximum length of the string before it either overflows or is clipped off.  Setting this to less than or equal to 0 will have it equal the screen size. 
 * @param overflow   - A boolean value that specifies whether the text should be clipped if it extends beyond the bounds of the graphics context.
 */
void drawChars(char* str, int x, int y, int max_length, bool overflow, int layer)
{

    max_length = max_length > 0 ? max_length > SCREEN_SIZE_X ? SCREEN_SIZE_X : max_length : SCREEN_SIZE_X; // Holy shit

    for (int i = 0; str[i] != '\0'; i++)
    {
        drawSingleChar(str[i], x, y, layer);

        // Increase position
        int x_increase = (x + 1) % max_length;
        int y_increase = (x + 1) / max_length;


        // Handle overflow
        if (y_increase > 0)
        {
            if (overflow)
            {
                y = y + y_increase;
            }
            else 
            {
                break;
            }
        }

        x = x_increase;

        // Check for max height being exceded
        if (y >= SCREEN_SIZE_Y)
            break; 
    }
}

// Draws a single character at position.
// Overrides any characters currently positioned there.
void drawSingleChar(char c, int x, int y, int layer)
{
    screen_layers[getScreenPos(x, y)][layer] = c;
    updateFinalScreenAtPos(x, y);
}

const int getScreenSizeX()
{
    return SCREEN_SIZE_X;
}

const int getScreenSizeY()
{
    return SCREEN_SIZE_Y;
}

// Returns array position of X and Y.
int getScreenPos(int x, int y)
{
    int newline_shifter = y > 0 ? 1: 0; // Maybe not the best approach but it works
    return x + (y*SCREEN_SIZE_X) + newline_shifter; 
}

// Returns -1 if none are empty
int highestEmptyScreenLayer(int x, int y)
{
    char* pixel = screen_layers[getScreenPos(x, y)];
    int layer = -1;
    for (int i = 0; i < LAYERS; i++)
    {
        if (pixel[i] == '\0')
        {
            layer = i;
        }
    }

    return layer;
}

void updateFinalScreenAtIndex(int element_index)
{
    for (int i = 0; i < LAYERS; i++)
    {
        if (screen_layers[element_index][i] != '\0')
        {
            screen_final[element_index] = screen_layers[element_index][i];
        }
    }
}

// Updates single char on final screen array
void updateFinalScreenAtPos(int x, int y)
{
    int element_index = getScreenPos(x, y);
    updateFinalScreenAtIndex(element_index);
}

void drawScreen(void)
{
    printf("%s", screen_final);
}