#ifndef MAP
#define MAP

#include "datatypes.h"

void initMapStats(void);
bool inMapBounds(position pos);
tile* getMapTile(position pos);

#endif