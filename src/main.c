#include "main.h"
#include "helpers.h"
#include "sim.h"
#include "settlements.h"
#include "map.h"
#include "borders.h"

#include "window.h"

#include "units.h"
#include "population.h"

#include <stdio.h>

int unsigned ticks = 0;
int seed = 0;

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
    initWindow();

    // Handle tick events
    while (!shouldWindowClose())
    {
        runSim();
        updateWindow();
        delay(TICK_DELAY);
        ticks++;
    }

    terminateWindow();
    return 0;
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
    //for (int i = 0; i < NAT_AMOUNT; i++)
    //{
        //addSettlement(nations[i].name, initSettlementPosition(i), NULL);
    //}

    unsigned int size = 0;
    citizen** cits = malloc(sizeof(citizen*)*size);
    if (cits == NULL) exit(1);

    position start_pos = newPosition(8, 8);

    addRandomCitizens(15, start_pos, &cits, &size);

    unit* new = newUnit(start_pos, 1, SETTLER, NULL, 1, cits, size);
    addToUnitArray(&nations[1].units, &nations[1].units_amt, new);
}

// TODO A bit lazy, maybe try something else for final release
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