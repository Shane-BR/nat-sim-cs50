#ifndef POP
#define POP

#include "datatypes.h"
#include <stdint.h>

void addRandomCitizens(int num, position init_pos, citizen*** arr, unsigned int* size);
void addCitizen(citizen* cit, citizen*** arr, unsigned int* size);
void calcMetToday(citizen* cit, settlement* stl);
void healthCheck(citizen* cit, settlement* stl); // Runs chances for disease and injury
void updateRelationship(citizen *cit, settlement* stl);
uint8_t getAgeHealthCurve(uint8_t age);

#endif