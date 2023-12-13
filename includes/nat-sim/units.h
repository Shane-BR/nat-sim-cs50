#ifndef NS_UNITS
#define NS_UNITS
#include "datatypes.h"


unit* newUnit(position pos, uint8_t nation, UnitClass unit_class, list_node* path, int move_speed, citizen** citizens, unsigned int cit_amt);
void updateUnit(unit* unit);
void updatePath(unit* unit, list_node* path);
void gotoPos(unit* unit, position pos);

#endif