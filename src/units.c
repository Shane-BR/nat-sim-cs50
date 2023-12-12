#include "units.h"
#include "helpers.h"
#include "map.h"
#include "pathfinder.h"
#include "settlements.h"

#include <stdint.h>

void followPath(unit* unit);

extern tile map[MAP_SIZE][MAP_SIZE];
extern nation nations[NAT_AMOUNT];


unit* newUnit(position pos, uint8_t nation, UnitClass unit_class, list_node* path, int move_speed, citizen** citizens, unsigned int cit_amt)
{
    unit* new = malloc(sizeof(unit));
    new->position = pos;
    new->nation = nation;
    new->unit_class = unit_class;

    new->path = NULL;
    updatePath(new, path);
    new->current_traversability = 0;

    new->move_speed = move_speed;
    new->citizens = citizens;
    new->cits_amt = cit_amt;

    return new;
}

void updateUnit(unit* unit)
{
    followPath(unit);
}

// Updates the path on the unit
void gotoPos(unit* unit, position pos)
{
    list_node* path = getPath(unit->position, pos, MAP_SIZE, map);
    updatePath(unit, path);
}

void updatePath(unit* unit, list_node* path)
{
    if (path != NULL)
    {
        if (unit->path != NULL) 
        {
            eraseLinkedList(unit->path);
            unit->path = NULL;
        }

        unit->path = path;
        
        position start = *(position*)(path->data);
        unit->current_traversability = getMapTile(start)->traversability;
    }
}

void settleOnPosition(unit* unit)
{
    if (unit->unit_class != SETTLER) return;

    if (getMapTile(unit->position)->ruling_nation == -1 || getMapTile(unit->position)->ruling_nation == unit->nation)
    {
        addSettlement(getNationName(unit->nation), unit->position, unit->citizens, unit->cits_amt);
    
        // Remove unit
        removeFromUnitArray(&nations[unit->nation].units, &nations[unit->nation].units_amt, unit);
    }
}

// LIST NODES MUST CONTAIN POSITIONS ONLY
void followPath(unit* unit)
{
    if (unit->path == NULL) return;

    int i = unit->move_speed;
    while (i-- > 0 && unit->path != NULL) 
    {
        if (unit->current_traversability-- <= 0)
        {
            position next_pos = *(position*)(unit->path->data);
            unit->position = next_pos;
            unit->path = unit->path->next;
            unit->current_traversability = getMapTile(next_pos)->traversability;

            // Move citizens if any
            for (int i = 0; i < unit->cits_amt; i++)
            {
                unit->citizens[i]->position = unit->position;
            }
        }
    }

    if (unit->path == NULL) 
    {
        settleOnPosition(unit);
        eraseLinkedList(unit->path);
    }
}