#include "settlements.h"
#include "constants.h"
#include "helpers.h"
#include "borders.h"
#include "population.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern tile map[MAP_SIZE][MAP_SIZE];
extern nation nations[NAT_AMOUNT];
extern unsigned int ticks;
settlement settlements[MAP_SIZE*MAP_SIZE]; // Simple hash table of settlements

void addSettlement(char* nation, position pos)
{
    int i = getNationIndex(nation);
    settlement new_stl = initSettlement(pos, nation);

    settlements[tileHash(pos)] = new_stl;

    updateSettlementBorders(&settlements[tileHash(pos)], 0);
}

settlement initSettlement(position pos, char* ruling_nation)
{
    settlement s;
    int8_t nat_index = getNationIndex(ruling_nation);

    s.position.x = pos.x;
    s.position.y = pos.y;

    map[s.position.y][s.position.x].ruling_nation = nat_index; // Set the tile to belong to the nation passed in
    s.nation = nat_index; // Also set the settlement to belong to that nation

    s.active = true;

    s.level = 0;
    s.local_population = 0; // Set after init process
    s.local_morale = 100;
    s.local_infrastructure = 10;
    s.cultivation_efficiency = 0;
    s.food = 140; // Lasts 3.5 days at population of 10

    s.borders = NULL;
    s.death_list = NULL;

    // Temp variable
    int numOfCitizens = 101;

    s.citizens = malloc(sizeof(citizen*) * numOfCitizens);

    if (s.citizens == NULL) exit(1);
    
    addRandomCitizens(numOfCitizens, &s);

    return s;
}

// Returns a pointer to the settlement, if it doesn't exists, return NULL
settlement* getSettlementFromPosition(position pos)
{
    int hash_code = tileHash(pos);
    return settlements[hash_code].active ? &settlements[hash_code] : NULL;
}

// Free after use
settlement** getAllSettlements(int8_t nation_index)
{
    int len = MAP_SIZE*MAP_SIZE;
    settlement** stls = malloc(sizeof(settlement*)*len);

    for (int i = 0, stls_count = 0; i < len; i++)
    {
        if (nation_index == settlements[i].nation)
        {
            stls[stls_count++] = &settlements[i];
        }
    }  
    return stls;
}

void calcCultivationEfficiency(settlement* stl)
{
    // lRC = (lM + lI) / 2
    stl->cultivation_efficiency = (stl->local_morale + stl->local_infrastructure) / 2;
}

void runPopulationChecks(settlement* stl)
{
    for (int i = 0; i < stl->local_population; i++)
    {
        citizen* cit = stl->citizens[i]; 

        // Age population
        if (ticks % (365*TICKS_PER_DAY) == 0)
            cit->age++;

        // Update relationship stats and calc breakup
        updateRelationship(cit, stl);

        calcMetToday(cit, stl);

        // Run health checks
        healthCheck(cit, stl);
    }
}

void updateSettlementStats(settlement* stl)
{
    // Update food
    int net_food = getNetFoodProduced(*stl);
    stl->food = clamp(stl->food + net_food, 0, INFINITY);

    // Update materials

}

// Removes citizen from border if possible
void removeCitFromBorder(citizen* cit, settlement* stl)
{
    border* b = findWorkBorder(cit, stl);

    cit->citizen_class = NONE;
    cit->position = stl->position;

    // Cit not in border
    if (b == NULL)
        return;

    int index = findWorkerIndexInBorder(b, cit);

    // Shuffle workers order in array
    for (int i = index; i < b->workers_count; i++)
    {
        b->workers[i] = b->workers[i+1];
    }

    b->workers[b->workers_count-1] = NULL;
    b->workers_count--;
}

int getBorderRadius(settlement stl)
{
    return (stl.level == 0 ? TOWN_BORDER_RADIUS : stl.level == 1 ? CITY_BORDER_RADIUS : 0);
}

int getBorderArea(settlement stl)
{
    return pow(getBorderRadius(stl), 2)-1;
}

int getNetFoodProduced(settlement stl)
{
    int food_demand = stl.local_population*MEALS_PER_DAY; 
    int food_produced = 0;

    for (int i = 0; i < getBorderArea(stl); i++)
    {
        food_produced = food_produced + stl.borders[i].workers_count * getBorderWorkerProduction(stl, stl.borders[i], FOOD);
    }

    return food_produced - food_demand;
}

// Checks to see if the settlement can upgrade/downgrade
void canChangeSettlementLevel(settlement* stl)
{

    int prev_radius = getBorderRadius(*stl);

    if (stl->local_population >= 100 && stl->level != CITY)
    {
        stl->level = CITY;
        updateSettlementBorders(stl, prev_radius);
    }
    else if (stl->local_population < 100 && stl->level != TOWN)
    {

        stl->level = TOWN;
        updateSettlementBorders(stl, prev_radius);
    }
}