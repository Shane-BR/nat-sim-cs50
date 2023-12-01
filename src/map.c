#include "constants.h"
#include "helpers.h"
#include "sprite_renderer.h"

tile map[MAP_SIZE][MAP_SIZE];

void initMapStats(void)
{
    for (int y = 0; y < MAP_SIZE; y++)
    {
        for (int x = 0; x < MAP_SIZE; x++)
        {
            map[y][x].food_abundance = randomInt(0, 255);
            map[y][x].material_abundance = randomInt(0, 255);
            map[y][x].traversability = randomInt(1, 4);
            map[y][x].survivability = randomInt(0, 255);
            map[y][x].ruling_nation = -1;

            // Set the tiles position stat
            map[y][x].position = newPosition(x, y); // May look dumb, but it allows us to get the position if we only have acess to the tile.
        }
    }
}

bool inMapBounds(position pos)
{
    return pos.y >= 0 && pos.y <= MAP_SIZE-1 && pos.x >= 0 && pos.x <= MAP_SIZE-1;
}

// Returns NULL if out of bounds
tile* getMapTile(position pos)
{
    return  inMapBounds(pos) ? &map[pos.y][pos.x] : NULL;
}