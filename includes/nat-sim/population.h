#ifndef NS_POPULATION
#define NS_POPULATION

#include "datatypes.h"
#include <stdint.h>

void addRandomCitizens(int num, position init_pos, citizen*** arr, unsigned int* size, unsigned int* mem_block_size);
void addCitizen(citizen* cit, citizen*** arr, unsigned int* size, unsigned int* mem_block_size);
void calcMetToday(citizen* cit, settlement* stl);
void healthCheck(citizen* cit, settlement* stl); // Runs chances for disease and injury
void updateRelationship(citizen *cit, settlement* stl);
uint8_t getAgeHealthCurve(uint8_t age);
void damageCitizen(citizen *cit, uint8_t amt, CitDamageSource cause);

#endif