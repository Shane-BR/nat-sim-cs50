#ifndef BORDERS
#define BORDERS

#include "datatypes.h"
#include <stdint.h>

void updateSettlementBorders(settlement* stl);
border* findWorkBorder(citizen* cit, settlement* stl);
int findWorkerIndexInBorder(border* border, citizen* cit);
unsigned int getBorderWorkerProduction(settlement stl, border border, uint8_t resource_type);
border* getBorderFromPosition(position pos);

#endif