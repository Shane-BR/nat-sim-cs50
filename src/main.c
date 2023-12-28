#include "main.h"
#include "helpers.h"
#include "sim.h"
#include "settlements.h"
#include "map.h"
#include "borders.h"

#include "window.h"
#include "input.h"

#include "units.h"
#include "population.h"
#include "text_renderer.h"
#include "sim_log.h"
#include "sim_time.h"

#include <stdio.h>

int seed = 0;

static float tick_timer = 0.0f;

extern nation nations[NAT_AMOUNT];

void cleanUp(void);

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

    initWindow();
    initMapStats();
    initNations();
    initSim();
    
    // Introduction log
    char welcome[64] = {'\0'};
    sprintf(welcome, "        WELCOME TO NAT-SIM %s", NS_VERSION);

    addLog("");
    addLog("");
    addLog("----------------------------------------------");
    addLog("----------------------------------------------");
    addLog(welcome);
    addLog("----------------------------------------------");
    addLog("----------------------------------------------");
    addLog("");

    // Handle tick events
    while (!shouldWindowClose())
    {

        tick_timer += calcDeltaTime();        
        if (tick_timer >= (1.0f / getTPS()))
        {
            runSim();

            tick_timer -= (1.0f / getTPS());
            incrementTicks();
        }

        updateWindow();
    }

    // Free heap
    cleanUp();

    printToLogsFile();
    terminateWindow();
    return 0;
}

void initNations(void)
{
    nations[0].name = "Elves";
    nations[1].name = "Men";
    nations[2].name = "Goblins";
    nations[3].name = "Dwarves";

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

        nations[i].death_list = NULL;

        // Units
        unsigned int mem_cap = 128;

        nations[i].units_amt = 0;
        nations[i].units_mem_capacity = mem_cap;

        nations[i].units = malloc(sizeof(unit*)*mem_cap);
        if (nations[i].units == NULL)
        {
            printf("Unable to allocate memory for nations units array.");
            exit(1);
        }

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
    unsigned int amount_cits = 15;
    citizen** cits = malloc(sizeof(citizen*)*amount_cits);
    if (cits == NULL) exit(1);

    position start_pos = {8, 8};

    addRandomCitizens(amount_cits, start_pos, &cits, &size, &amount_cits);

    unit* new = newUnit(start_pos, 1, SETTLER, NULL, 1, cits, size);
    addToDynamicPointerArray((void***)&nations[1].units, &nations[1].units_amt, new, &nations[1].units_mem_capacity);
}

void cleanUp(void)
{
    // Clean up any units
    for (int n = 0; n < NAT_AMOUNT; n++)
    {
        for (int u = 0; u < nations[n].units_amt; n++)
        {
            freeUnit(nations[n].units[u]);
        }

        free(nations[n].units);

        // Free death list
        eraseLinkedList(&nations[n].death_list, true);
    }

    // Clean up settlements
    for (int i = 0; i < MAP_SIZE*MAP_SIZE; i++)
    {
        settlement* stl = getSettlementFromIndex(i);

        if (stl == NULL)
            continue;

        removeSettlement(stl);
    }

    freeBindings();
}