#include "units.h"
#include "helpers.h"
#include "pathfinder.h"
#include <stdint.h>

void followPath(unit* unit);
void updatePath(unit* unit, list_node* path);

extern tile map[MAP_SIZE][MAP_SIZE];


unit* newUnit(position pos, uint8_t nation, UnitClass unit_class, list_node* path, int move_speed, citizen** citizens)
{
    unit* new = malloc(sizeof(unit));
    new->position = pos;
    new->nation = nation;
    new->unit_class = unit_class;

    new->path = NULL;
    updatePath(new, path);
    
    new->move_speed = move_speed;
    new->citizens = citizens;

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
        unit->current_traversability = getMapTile(start).traversability;
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
            unit->current_traversability = getMapTile(next_pos).traversability;
        }
    }

    if (unit->path == NULL) 
        eraseLinkedList(unit->path);
}