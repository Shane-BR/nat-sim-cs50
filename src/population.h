#ifndef POP
#define POP

#include "datatypes.h"
#include <stdint.h>

void addRandomCitizens(int num, settlement* stl);
void addCitizen(citizen* cit, settlement* stl);
void calcMetToday(citizen* cit, settlement* stl);
void healthCheck(citizen* cit, settlement* stl); // Runs chances for disease and injury
void updateRelationship(citizen *cit, settlement* stl);
uint8_t getAgeHealthCurve(uint8_t age);

#endif