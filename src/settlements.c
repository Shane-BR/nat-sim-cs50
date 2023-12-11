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

int getGrossResourceProduced(settlement stl, int resource_type);

void addSettlement(char* nation, position pos, citizen** citizens, int cit_size)
{
    int i = getNationIndex(nation);
    settlement new_stl = initSettlement(pos, nation, citizens, cit_size);

    settlements[tileHash(pos)] = new_stl;

    updateSettlementBorders(&settlements[tileHash(pos)], 0);
}

settlement initSettlement(position pos, char* ruling_nation, citizen** citizens, int cit_size)
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
    s.local_infrastructure = 0;
    s.cultivation_efficiency = 0;
    s.food = 140; // Lasts 3.5 days at population of 10

    s.borders = NULL;
    s.death_list = NULL;

    if (citizens == NULL)
    {
        int numOfCitizens = 5;

        s.citizens = malloc(sizeof(citizen*) * numOfCitizens);

        if (s.citizens == NULL) exit(1);
    
        addRandomCitizens(numOfCitizens, s.position, &s.citizens, &s.local_population);
    }
    else 
    {
        s.citizens = citizens;
        s.local_population = cit_size;
    }

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

    // Update gross food product.
    // Food is deducted in population.c by individual citizens
    stl->food = clamp(stl->food + 
        getGrossResourceProduced(*stl, FOOD), 0, INFINITY);

    // Update materials.  Deduct later.
    stl->materials = clamp(stl->materials + 
        getGrossResourceProduced(*stl, MATERIALS), 0, INFINITY);

    // Calc infrastructure growth/decline

    // Max amount of craftsmen to reach highest target rate
    const int MAX_CRAFTSMEN = 200; 

    int craftsmen = 0;

    // Count all craftsmen
    for (int i = 0; i < stl->local_population; i++)
    {
        if (stl->citizens[i]->citizen_class == CRAFTSMAN) 
            craftsmen++;
    }

    // Update local infrastructure target
    // Scale with population
    int li_target = (craftsmen / (double)MAX_CRAFTSMEN) * UINT8_MAX;

    // Max amount of mat needed this tick to achieve full speed infrastructure growth
    int max_mat_needed = MAX_MAT_PRODUCED_PER_WORKER * craftsmen;

    // Take as much as 90%, to integer, of materials.
    int take_amt = clamp(stl->materials * 0.9f, 0, max_mat_needed);
    stl->materials = stl->materials - take_amt;

    // Max growth per tick
    double max_growth_tick = 0.035;  // 0.035 = 255 over 10 years at max growth speed 
    
    double growth_tick;

    if (max_mat_needed < 1 || take_amt < 1)
    {
        growth_tick = max_growth_tick;
    }
    else 
    {
        growth_tick = (take_amt / (double)max_mat_needed) * max_growth_tick;
    }

    uint8_t clamp_min = 0;
    uint8_t clamp_max = UINT8_MAX;

    // Are we decreasing infrastructure?
    if (li_target >  stl->local_infrastructure)
    {
        clamp_max = li_target;
    }
    else if (li_target < stl->local_infrastructure) 
    {
        growth_tick = growth_tick * -1;
        clamp_min = li_target;
    }

    stl->local_infrastructure = clamp(stl->local_infrastructure + growth_tick, clamp_min, clamp_max);
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

int getEstimateNetFoodProduced(settlement stl)
{
    int food_demand = (stl.local_population*MEALS_PER_DAY) / TICKS_PER_DAY; 
    int food_produced = getGrossResourceProduced(stl, FOOD);

    return food_produced - food_demand;
}

int getGrossResourceProduced(settlement stl, int resource_type)
{
    int produce = 0;

    for (int i = 0; i < getBorderArea(stl); i++)
    {
        if (stl.borders[i] == NULL)
            continue;

        produce = produce + stl.borders[i]->workers_count * getBorderWorkerProduction(stl, *stl.borders[i], resource_type);
    }

    return produce;
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