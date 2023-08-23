#ifndef STL
#define STL

#include "datatypes.h"

void addSettlement(char* nation, position pos);
settlement initSettlement(position pos, char* ruling_nation);
settlement* getSettlementFromPosition(position pos);
settlement** getAllSettlements(int8_t nation_index);
void calcCultivationEfficiency(settlement* stl);
void runPopulationChecks(settlement* stl);
void removeCitFromBorder(citizen* cit, settlement* stl);
int getBorderSize(settlement stl);
int getNetFoodProduced(settlement stl);
void canChangeSettlementLevel(settlement* stl);

#endif