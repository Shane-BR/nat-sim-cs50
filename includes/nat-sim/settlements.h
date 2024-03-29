#ifndef NS_STL
#define NS_STL

#include "datatypes.h"

void addSettlement(char* nation, position pos, citizen** citizens, int cit_size);
void removeSettlement(settlement* stl);
settlement initSettlement(position pos, char* ruling_nation, citizen** citizens, int cit_size);
settlement* getSettlementFromPosition(position pos);
settlement* getSettlementFromIndex(const int index);
settlement** getAllSettlements(int8_t nation_index);
void calcCultivationEfficiency(settlement* stl);
void runPopulationChecks(settlement* stl);
void updateSettlementStats(settlement* stl);
void removeCitFromBorder(citizen* cit, settlement* stl);
int getBorderRadius(settlement stl);
int getBorderArea(settlement stl);
int getEstimateNetFoodProduced(settlement stl);
void launchSettlerUnitFromSettlement(settlement* stl);
void canChangeSettlementLevel(settlement* stl);

#endif