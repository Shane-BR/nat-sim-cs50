// Handles updates on tick

#include "sim.h"
#include "datatypes.h"
#include "constants.h"
#include "settlements.h"
#include "nation_ai.h"

extern unsigned int ticks;
extern tile map[MAP_SIZE][MAP_SIZE]; // 2D array for easy lookup
extern nation nations[NAT_AMOUNT];
extern settlement settlements[MAP_SIZE*MAP_SIZE];

void calcSettlementStats(void);
void runNationManagerAI(settlement* stl);


void runSim(void)
{
    calcSettlementStats();
}

void runNationManagerAI(settlement* stl)
{
    manageFood(stl, getNetFoodProduced(*stl));
}

void calcSettlementStats(void)
{
    // Reset nation stats ready for recalculation
    // TODO redo this system
    for (int n = 0; n < NAT_AMOUNT; n++)
    {
        nations[n].population = 0;
        nations[n].morale = 0;
        nations[n].infrastructure = 0;
    }

    // Run calculations
    for (int i = 0; i < MAP_SIZE*MAP_SIZE; i++)
    {
        settlement* s = &settlements[i];

        // Should settlement be active?
        s->active = s->local_population > 0;

        if (s->active)
        {
            // Handle calculations
            runPopulationChecks(s);
            calcCultivationEfficiency(s);
            runNationManagerAI(s);
            updateSettlementStats(s);
            canChangeSettlementLevel(s);

            // Set stats at nation level 
            // TODO REDO
            int n = s->nation;
            nations[n].population           += s->local_population;
            nations[n].morale               += s->local_morale;
            nations[n].infrastructure       += s->local_infrastructure;
        }
    }
}
