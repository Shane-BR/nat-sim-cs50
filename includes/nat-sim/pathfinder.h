#ifndef NS_PATHFINDER
#define NS_PATHFINDER
#include "datatypes.h"

list_node* getPath(position start, position goal, int search_area_size, tile search_area[search_area_size][search_area_size]);

#endif