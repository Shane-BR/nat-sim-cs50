#include "nation_ai.h"
#include "constants.h"
#include "borders.h"
#include "settlements.h"
#include "helpers.h"
#include "map.h"
#include "pathfinder.h"
#include "population.h"
#include "units.h"
#include <stdint.h>

void assignCitizensToWorkBorder(settlement* stl, int amount, border* border_tile, int max_class_priority);
bool organiseMoreFoodWorkers(settlement* stl, int food_net);
void assignCitizensFromBorderToWorkSettlement(settlement* stl, int amount, border* from_border);
void organiseSurplusWorkersFromBorders(settlement* stl, int surplus, uint8_t resource_type);
border* worstProducingBorder(settlement* stl, uint8_t resource_type);
tile getBestSettlerTile(position origin, int unit_nat);

extern tile map[MAP_SIZE][MAP_SIZE];

// Decision tree based system for managing food.
// food_net = food netted this tick.  Negitive if demand outways supply.
// food_net is also after food traded
void manageFood(settlement* stl, int food_net)
{
    int month_of_food = stl->local_population * 28;

    // if food intake not meeting demand and cannot reorginise workers
    if (food_net < 0 && stl->food < month_of_food)
    {
        if (!organiseMoreFoodWorkers(stl, -food_net)) 
        {
        
            // If nation has more than one settlement
                // Request food in next trade caravan
            // Else if One of the other nations isn't at war with this nation
                // Request to trade food for other goods
            // Else
                // Disband a settlement party to a better location
        }
    }

    // If too many workers gathering food
    else if (food_net > 0 && stl->food > month_of_food) 
    {
        organiseSurplusWorkersFromBorders(stl, food_net, FOOD);
    }
}

void manageSettlerUnit(unit* settler)
{

    if (settler->unit_class != SETTLER)
        return;

    // FIXME This is a simplified version.
    // I need to get this done damn it.

    if (settler->path == NULL)
    {
        // Find best uncontrolled tile
        tile best = getBestSettlerTile(settler->position, settler->nation);

        // Get path to tile
        list_node* path = getPath(settler->position, best.position, MAP_SIZE, map);
    
        // Update path
        updatePath(settler, path);
    }
}

tile getBestSettlerTile(position origin, int unit_nat)
{
    tile best;
    int cur_best = 0;

    // Loop through all positions
    for (int y = 0; y < MAP_SIZE; y++) 
    {
        for (int x = 0; x < MAP_SIZE; x++) 
        {
            tile* search_tile = getMapTile((position){x, y});

            if (search_tile == NULL) continue;

            int tr = 0;
            
            // Combine resources of all potential border tiles to get the total
            for (int i = 0; i < (CITY_BORDER_RADIUS*CITY_BORDER_RADIUS); i++)
            {
                int tx = x + ((i % CITY_BORDER_RADIUS) - CITY_BORDER_RADIUS/2);
                int ty = y + ((i / CITY_BORDER_RADIUS) - CITY_BORDER_RADIUS/2);
                tile* t = getMapTile((position){tx, ty});

                if (t == NULL || t->ruling_nation != -1)
                    continue;

                tr += t->food_abundance + t->material_abundance + t->survivability;
            }
            

            int score = tr - (distanceBetweenPositions(search_tile->position, origin) / (MAP_SIZE-1) * UINT8_MAX) *
                                (CITY_BORDER_RADIUS*CITY_BORDER_RADIUS);
            
            if (score > cur_best)
            {
                cur_best = score;
                best = *search_tile;
            }
        }
    }
    
    return best;
}

void manageMaterials(settlement* stl)
{
    
}

void manageSettlementWorkers(settlement* stl)
{
    // Convert all working age NONE classed citizens to CRAFTSMEN

    // Find all NONE citizens
    for (int i = 0; i < stl->local_population; i++)
    {
        citizen* cit = stl->citizens[i];
        if (cit->citizen_class == NONE && canCitizenWork(cit))
        {
            // Convert to CRAFTSMEN
            cit->citizen_class = CRAFTSMAN;
        }
    }
}

// Returns true if able to meet required food
bool organiseMoreFoodWorkers(settlement* stl, int food_required)
{
    bool out_of_reassignable_workers = false;
    // While food requirement not met and while we still are able to reassign workers
    while (food_required > 0 && !out_of_reassignable_workers) {
        // Find the highest food_abundance border tile that isn't full
        
        border init;
        tile init_tile;
        init.tile = & init_tile;
        init_tile.food_abundance = 0;

        border* h_food = &init;
        h_food->tile->food_abundance = 0;

        for (int i = 0; i < getBorderArea(*stl)-1; i++)
        {
            if (stl->borders[i] == NULL)
                continue;

            if (stl->borders[i]->tile->food_abundance > h_food->tile->food_abundance 
                && stl->borders[i]->workers_count < MAX_BORDER_WORKERS)
            {
                h_food = stl->borders[i];
            }
        }

        // How much food can one worker produce per tick
        int food_per_worker = getBorderWorkerProduction(*stl, *h_food, FOOD);        

        // Figure out how many extra workers are needed for this tile
        int amt_workers_needed = 
            (food_required == 0 || food_per_worker == 0) ? 0 :
            (int)(food_required / food_per_worker) + 1 + h_food->workers_count >= MAX_BORDER_WORKERS ? 
            MAX_BORDER_WORKERS-h_food->workers_count : 
            (int)(food_required / food_per_worker) + 1;

        // Assign workers to new border tile
        int old_worker_count = h_food->workers_count;
        assignCitizensToWorkBorder(stl, amt_workers_needed, h_food, GATHERER);

        // Check if workers added
        out_of_reassignable_workers = old_worker_count - h_food->workers_count == 0;

        // Update the food_required stat
        food_required = food_required - amt_workers_needed*food_per_worker;
    }

    return food_required > 0;
}

// Assigns an amount of workers to work a border tile area.
void assignCitizensToWorkBorder(settlement* stl, int amount, border* border_tile, int max_class_priority)
{
    while (amount-- > 0)
    {
        int class_priority = NONE;
        for (int i = 0; i < stl->local_population; i++)
        {

            citizen* cit = stl->citizens[i];

            if (border_tile->workers_count >= MAX_BORDER_WORKERS)
                return;

            bool already_assigned = false;
            for (int j = 0; j < border_tile->workers_count; j++)
            {
                if (border_tile->workers[j] == cit) 
                {
                    already_assigned = true;
                    break;
                }
            }

            bool added_cit = false;
            if (!already_assigned && 
                canCitizenWork(cit) &&
                cit->citizen_class == class_priority)
            {

                // prioritize NONE classes first
                // NONE - CRAFTSMAN - MILITARY - GATHERER

                // Add to worker array
                if (cit->citizen_class != GATHERER)
                    cit->citizen_class = GATHERER;
                else 
                {
                    // Move from old border tile to new
                    removeCitFromBorder(cit, stl);
                }

                border_tile->workers[border_tile->workers_count++] = cit;
                cit->position = border_tile->tile->position;
                added_cit = true;
            }

            // Lazy approach fuck you
            if (i == stl->local_population-1 && class_priority < max_class_priority)
            {
                class_priority++;
                i = 0;
            }

            if (added_cit)
                break;

        }
    }
}

// Moves workers from border areas back to settlement acording to the surplus of resources.
// Will attempt to get the surplus as close to zero as posssible.
void organiseSurplusWorkersFromBorders(settlement* stl, int surplus, uint8_t resource_type)
{
    bool can_reduce_workers = true; // False when removing any more workers will result in a resource deficit.
    // We can still reorginise workers if total food is greater than a weeks worth
    while (surplus > 0 && can_reduce_workers)
    {
        // Get worst border with workers
        border* worst = worstProducingBorder(stl, resource_type);
        
        if (worst == NULL)
            return;

        // How much food can one worker produce per tick
        int r_per_worker =  getBorderWorkerProduction(*stl, *worst, resource_type);

        // Remove enough workers to reduce surplus or empty border area
        int amt_workers_to_remove = surplus / r_per_worker > worst->workers_count ? 
            worst->workers_count :
            surplus / r_per_worker;

        if (amt_workers_to_remove <= 0) return;

        assignCitizensFromBorderToWorkSettlement(stl, amt_workers_to_remove, worst);

        // Update surplus
        surplus = surplus - amt_workers_to_remove * r_per_worker;

        // If we cannot reassign any more workers without causing a resource deficit.
        can_reduce_workers = surplus < r_per_worker;

    }
}

// Sets citizens to work at the settlement as a CRAFTSMAN
void assignCitizensFromBorderToWorkSettlement(settlement* stl, int amount, border* from_border)
{
    amount = amount > from_border->workers_count ? from_border->workers_count : amount;
    while (amount-- > 0)
    {
        // Doesn't really matter where we grab the workers for now
        citizen* cit = from_border->workers[amount-1 < 0 ? 0 : amount-1];
        removeCitFromBorder(cit, stl);
        cit->citizen_class = CRAFTSMAN;
    }
}

/**
 * Finds the border with the lowest production of a given resource type.
 *
 * stl = Pointer to the settlement structure.
 * resource_type = The type of resource to check for (Combined if non-resource number passed).
 * Returns a pointer to the border with the lowest production.
 */
border* worstProducingBorder(settlement* stl, uint8_t resource_type)
{
    border* out = NULL;
    int out_resources = INT32_MAX;
    for (int i = 0; i < getBorderArea(*stl)-1; i++)
    {
        int tile_resources = -1;
        border* border = stl->borders[i];

        if (border == NULL)
            continue;

        // TODO make this more dynamic
        if (resource_type == FOOD)
        {
            tile_resources = border->tile->food_abundance;
        }
        else if (resource_type == MATERIALS)
        {
            tile_resources = border->tile->material_abundance;
        }
        else 
        {
            tile_resources = border->tile->food_abundance + border->tile->material_abundance;
        }

        if (tile_resources < out_resources && border->workers_count > 0)
        {
            out = border;
        }
    }
    return out;
}