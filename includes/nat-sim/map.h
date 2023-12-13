#ifndef NS_MAP
#define NS_MAP

#include "datatypes.h"

void initMapStats(void);
bool inMapBounds(position pos);
tile* getMapTile(position pos);

#endif