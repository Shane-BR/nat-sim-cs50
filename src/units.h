#ifndef UNITS
#define UNITS
#include "datatypes.h"


unit* newUnit(position pos, uint8_t nation, UnitClass unit_class, list_node* path, int move_speed, citizen** citizens);
void updateUnit(unit* unit);
void gotoPos(unit* unit, position pos);

#endif