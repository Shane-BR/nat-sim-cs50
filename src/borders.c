#include <stdint.h>
#include <stdlib.h>
#include "stdio.h"

#include "borders.h"
#include "constants.h"
#include "helpers.h"
#include "settlements.h"

extern tile map[MAP_SIZE][MAP_SIZE];

void setBorders (int radius, int prev_radius, settlement* stl);


// Update a settlements borders.
void updateSettlementBorders(settlement* stl, int prev_border_radius)
{

    if (!stl->active) return;

    if (stl->level == 0) // Town
    {
        setBorders(TOWN_BORDER_RADIUS, prev_border_radius, stl);
        
    }
    else if (stl->level == 1) // City
    {
        setBorders(CITY_BORDER_RADIUS, prev_border_radius, stl);
    }
}

// Please note: this will casue a segmentation fault if grid[] doesn't have enough space for all the tiles.
// Sets borders for settlement.
// TODO Handle contested borders and border.
// FIXME Access violation when not assigning the max number of borders.
// TODO Eventually redo this whole system.
void setBorders (int radius, int prev_radius, settlement* stl)
{

    // If decreasing border radius
    if (radius < prev_radius)
    {
        int target_arr_size = radius*radius-1;

        for (int i = prev_radius*prev_radius-2; i >= target_arr_size; i--)
        {
            // Reset tile
            stl->borders[i].tile->ruling_nation = -1;

            // Reassign workers
            for (int j = 0; j < stl->borders[i].workers_count; j++)
            {
                citizen* worker = stl->borders[i].workers[j];

                if (worker != NULL) removeCitFromBorder(worker, stl);
            }
        }
    }

    // Then realloc border array
    stl->borders = realloc(stl->borders, sizeof(border)*((radius*radius)-1));

    if (stl->borders == NULL) exit(1);

    // If increasing border radius
    if (radius > prev_radius)
    {
        position center = stl->position;
        position tl_corner = newPosition(center.x - radius/2, center.y - radius/2);
        position br_corner = newPosition(center.x + radius/2, center.y + radius/2);

        int cur = prev_radius > 0 ? (prev_radius*prev_radius-1) : 0;

        for (int y = tl_corner.y; y <= br_corner.y; y++) 
        {
            for (int x = tl_corner.x; x <= br_corner.x; x++) 
            {
                // Is already in border array?
                if (prev_radius > 0)
                {
                    bool skip_tile = false;
                    for (int i = 0; i < prev_radius*prev_radius-1; i++)
                    {
                        if (comparePosition(stl->borders[i].tile->position, newPosition(x, y)))
                        {
                            skip_tile = true;
                            break;
                        }
                    }

                    if (skip_tile) continue;
                }

                if (!inMapBounds(newPosition(x, y)) || comparePosition(center, newPosition(x, y)))
                    continue;

                border new_border;
                new_border.tile = &map[y][x];
                new_border.workers_count = 0;

                map[y][x].ruling_nation = stl->nation; // Might be sussy idk, faster than looking up the linked list though
                stl->borders[cur++] = new_border;
            }
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

            for (int j = 0; j < getBorderArea(*stl); j++)
            {
                border* b = &stl->borders[j];
                
                if (comparePosition(b->tile->position, pos))
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

double getBorderWorkerProduction(settlement stl, border border, uint8_t resource_type)
{

    const double food_curve_midpoint = 20;
    const double food_curve_steepness = 0.1;

    const double mat_curve_midpoint = 20;
    const double mat_curve_steepness = 0.1;

    if (resource_type == FOOD) 
        return logistic
        (
        (border.tile->food_abundance * ((double)stl.cultivation_efficiency / UINT8_MAX)),
        MAX_FOOD_PRODUCED_PER_WORKER, 
        food_curve_steepness,
        food_curve_midpoint
        );

    else if (resource_type == MATERIALS)
        return logistic
        (
        (border.tile->material_abundance * ((double)stl.cultivation_efficiency / UINT8_MAX)),
        MAX_MAT_PRODUCED_PER_WORKER, 
        mat_curve_steepness,
        mat_curve_midpoint
        );
    
    return 0;
}