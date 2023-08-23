#include "main.h"
#include "helpers.h"
#include "borders.h"
#include "sim.h"
#include "settlements.h"
#include "borders.h"

#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int unsigned ticks = 0;
int seed = 0;
tile map[MAP_SIZE][MAP_SIZE]; // 2D array for easy lookup

extern nation nations[NAT_AMOUNT];
extern settlement settlements[MAP_SIZE*MAP_SIZE];

int main(int argc, char* argv[])
{   
    if (argc < 2)
    {
        seed = time(0);
    }
    else
    {

        int input = strToInt(argv[1]);
        if (input != 0)
        {
            seed = input;
        }
        else
        {
            printf("Usage executable.exe <seed>\n");
            return 1;
        }
    }

    srand(seed);

    initMapStats();
    initNations();
    initSim();

    // // TEMP
    // int x = 0;
    // int y = 0;
    // printf("X: ");
    // scanf_s("%i", &x);

    // printf("Y: ");
    // scanf_s("%i", &y);

    // printf("Ruling nation: %s\n", map[y][x].ruling_nation == -1 ? "None" : getNationName(map[y][x].ruling_nation));
    // printf("Resources: %i\n", map[y][x].resources);
    // printf("Traversability: %i\n", map[y][x].traversability);
    // printf("Survivability: %i\n", map[y][x].survivability);

    // Handle tick events
    while (true)
    {
        runSim();
        //draw();
        delay(TICK_DELAY);
        ticks++;
    }
}

void initMapStats(void)
{
    for (int y = 0; y < MAP_SIZE; y++)
    {
        for (int x = 0; x < MAP_SIZE; x++)
        {
            map[y][x].food_abundance = randomInt(0, 255);
            map[y][x].material_abundance = randomInt(0, 255);
            map[y][x].traversability = randomInt(0, 4);
            map[y][x].survivability = randomInt(0, 255);
            map[y][x].ruling_nation = -1;

            // Set the tiles position stat
            map[y][x].position = newPosition(x, y);
        }
    }
}

void initNations(void)
{
    nations[0].name = "elves";
    nations[1].name = "men";
    nations[2].name = "goblins";
    nations[3].name = "dwarves";

    for (int i = 0; i < NAT_AMOUNT; i++)
    {
        nations[i].population = 0; // Set by local settlements
        nations[i].health = 50;
        nations[i].trade = 50;
        nations[i].magic = 50;
        nations[i].morale = 0; // Set by local settlements
        nations[i].fear = 0;
        nations[i].infamy = 0;
        nations[i].infrastructure = 0; // Set by local settlements
        nations[i].military = 10;
        nations[i].money = 20;
        nations[i].artifacts = 0;
    }

}

void initSim(void)
{
    // Set and initialize a settlement for each nation
    for (int i = 0; i < NAT_AMOUNT; i++)
    {
        addSettlement(nations[i].name, initSettlementPosition(i));
    }
}

// TODO A bit lazy, maybe try another algorithm  for final release
position initSettlementPosition(int8_t nation_index)
{
    char* name = getNationName(nation_index);

    if (compareString(name, "elves"))
    {
        return newPosition(MAP_SIZE/4, MAP_SIZE/4);
    }
    else if (compareString(name, "men"))
    {
        return newPosition((MAP_SIZE-1)-(MAP_SIZE/4), MAP_SIZE/4);
    }
    else if (compareString(name, "goblins")) 
    {
        return newPosition(MAP_SIZE/4, (MAP_SIZE-1)-(MAP_SIZE/4));
    }
    else if (compareString(name, "dwarves")) 
    {
        return newPosition((MAP_SIZE-1)-(MAP_SIZE/4), (MAP_SIZE-1)-(MAP_SIZE/4));
    }

    return newPosition(0, 0);
}

void draw(void) // TODO remove and make frame buffer using OpenGL instead
{
    if (ticks / TICKS_PER_DAY % 365 != 0)
        return;

    system("cls");

    // Print header and stats above map view
    // Shitty code that doesn't take MAP_SIZE into account.  who cares, it's a temporary function
    printf("----------------------------YEAR: %i---------\n", ticks / TICKS_PER_DAY / 365);
    printf("SEED: %i\n", seed);
    printf("MPOP: %i\n", nations[1].population);

    for (int y = 0; y < MAP_SIZE; y++)
    {
        for (int x = 0; x < MAP_SIZE; x++)
        {
            if (map[y][x].ruling_nation == -1)
            {
                printf("%i", map[y][x].traversability);
            }
            else 
            {
                char nat_char = getNationName(map[y][x].ruling_nation)[0];
                switch (nat_char) 
                {
                    case 'e':
                        printf("\033[1;36m");
                        break;

                    case 'm':
                        printf("\033[1;34m");
                        break;

                    case 'g':
                        printf("\033[1;32m");
                        break;

                    case 'd':
                        printf("\033[1;31m");
                        break;
                }

                border* b = getBorderFromPosition(newPosition(x, y));
                if (getSettlementFromPosition(newPosition(x, y)) != NULL)
                    printf("%c", nat_char - 32);
                else if (b != NULL)
                    printf("%x", b->workers_count);
                else
                    printf("%i", map[y][x].traversability);
                
                printf("\033[0;0m");
            }

            printf("  ");
        }
        printf("\n");
    }
}