#include <stdint.h>
#include <stdlib.h>

#include "borders.h"
#include "constants.h"
#include "helpers.h"
#include "settlements.h"

extern tile map[MAP_SIZE][MAP_SIZE];

void setBorders (int radius, settlement* stl);

void updateSettlementBorders(settlement* stl) // Add territory for this settlement only
{

    if (!stl->active) return;

    if (stl->level == 0) // Town
    {
        setBorders(TOWN_BORDER_RADIUS, stl);
        
    }
    else if (stl->level == 1) // City
    {
        setBorders(CITY_BORDER_RADIUS, stl);
    }
}

// Please note: this will casue a segmentation fault if grid[] doesn't have enough space for all the tiles
void setBorders (int radius, settlement* stl)
{
    position center = stl->position;
    position tl_corner = newPosition(center.x - radius/2, center.y - radius/2);
    position br_corner = newPosition(center.x + radius/2, center.y + radius/2);

    stl->borders = realloc(stl->borders, sizeof(border)*((radius*radius)-1));
    uint8_t cur = 0;

    for (int y = tl_corner.y; y <= br_corner.y; y++)
    {
        for (int x = tl_corner.x; x <= br_corner.x; x++)
        {
            // TODO Contested borders will be owned by the nation with the largest Military stat
            if (map[y][x].ruling_nation != -1 || !inMapBounds(newPosition(x, y)) || (x == center.x && y == center.y))
                continue;

            border new_border;
            new_border.tile = map[y][x];
            new_border.workers_count = 0;

            map[y][x].ruling_nation = stl->nation; // Might be sussy idk, faster than looking up the linked list though
            stl->borders[cur++] = new_border;
        }
    }
}

// Finds the border where the citizen is working
border* findWorkBorder(citizen* cit, settlement* stl)
{
    for 
    (int i = 0; 
        i < (stl->level == 0 ? 
            TOWN_BORDER_RADIUS*TOWN_BORDER_RADIUS : 
            CITY_BORDER_RADIUS*CITY_BORDER_RADIUS)-1; i++)
    {
        for (int j = 0; j < stl->borders[i].workers_count; j++)
        {
            if (stl->borders[i].workers[j] == cit)
            {
                return &stl->borders[i];
            }
        }
    }

    return NULL;
}

border* getBorderFromPosition(position pos)
{
    int search_radious = (CITY_BORDER_RADIUS*CITY_BORDER_RADIUS);
    position search_pos;

    for (int i = 0; i < search_radious; i++)
    {

        search_pos.x = pos.x-CITY_BORDER_RADIUS/2 + i % CITY_BORDER_RADIUS;
        search_pos.y = pos.y-CITY_BORDER_RADIUS/2 + i / CITY_BORDER_RADIUS; 

        if(!inMapBounds(search_pos))
            continue;

        settlement* stl = getSettlementFromPosition(search_pos);
        if (stl != NULL && stl->active && map[search_pos.y][search_pos.x].ruling_nation == stl->nation)
        {
            for (int j = 0; j < getBorderSize(*stl); j++)
            {
                border* b = &stl->borders[j];
                
                if (comparePosition(b->tile.position, pos))
                {
                    return b;
                }
            }
        }
    }

    return NULL;
}

// Finds citizen index in border worker array
int findWorkerIndexInBorder(border* border, citizen* cit)
{
    for (int i = 0; i < border->workers_count; i++)
    {
        if (border->workers[i] == cit)
        {
            return i;
        }
    }
    return -1;
}

unsigned int getBorderWorkerProduction(settlement stl, border border, uint8_t resource_type)
{
    if (resource_type == FOOD) 
            return max((border.tile.food_abundance * (stl.cultivation_efficiency/UINT8_MAX)/UINT8_MAX)*MAX_FOOD_PRODUCED_PER_WORKER, 2);
    else if (resource_type == MATERIALS)
            return max((border.tile.material_abundance*(stl.cultivation_efficiency/UINT8_MAX)/UINT8_MAX)*MAX_MAT_PRODUCED_PER_WORKER, 2);
    
    return 0;
}