// Handles updates on tick

#include <stdlib.h>

#include "sim.h"
#include "datatypes.h"
#include "constants.h"
#include "settlements.h"
#include "nation_ai.h"
#include "helpers.h"
#include "map.h"
#include "units.h"

extern tile map[MAP_SIZE][MAP_SIZE]; // 2D array for easy lookup
extern nation nations[NAT_AMOUNT];
extern settlement settlements[MAP_SIZE*MAP_SIZE];

void calcSettlementStats(void);
void runNationManagerAI(settlement* stl);

void runSim(void)
{
    calcSettlementStats();

    // Update all units
    for (int i = 0; i < NAT_AMOUNT; i++)
    {
        for (int j = 0; j < nations[i].units_amt; j++)
        {
            unit* unit = nations[i].units[j];
            manageSettlerUnit(unit);
            updateUnit(unit);
        }
    }
}

void runNationManagerAI(settlement* stl)
{
    manageFood(stl, getEstimateNetFoodProduced(*stl));
    manageSettlementWorkers(stl);
}

void calcSettlementStats(void)
{
    // Reset nation stats ready for recalculation
    // TODO redo this system
    for (int n = 0; n < NAT_AMOUNT; n++)
    {

        // Add units before calculating settlement population
        int units_population = 0;
        for (int i = 0; i < nations[n].units_amt; i++)
        {
            units_population += nations[n].units[i]->cits_amt;
        }

        nations[n].population = units_population;
        nations[n].morale = 0;
        nations[n].infrastructure = 0;
    }

    // Run calculations
    for (int i = 0; i < MAP_SIZE*MAP_SIZE; i++)
    {
        settlement* s = &settlements[i];

        // active this tick?
        bool was_active = s->active;

        // Should settlement be active?
        s->active = s->local_population > 0;

        if (!s->active)
        {
            if (was_active)
            {
                // Set borders and stl back to neutral
                // Free all heap memory from stl
                for (int i = 0; i < getBorderArea(*s); i++)
                {
                    if (s->borders[i] == NULL)
                        continue;

                    s->borders[i]->tile->ruling_nation = -1;
                    free(s->borders[i]);
                }
                getMapTile(s->position)->ruling_nation = -1;
                free(s->borders);
                free(s->citizens);
            }
            continue;
        }

        // Handle calculations
        runPopulationChecks(s);
        calcCultivationEfficiency(s);
        runNationManagerAI(s);
        updateSettlementStats(s);
        canChangeSettlementLevel(s);

        // Set stats at nation level 
        int n = s->nation;
        nations[n].population           += s->local_population;
        nations[n].morale               += s->local_morale;
        nations[n].infrastructure       += s->local_infrastructure;
    }
}
