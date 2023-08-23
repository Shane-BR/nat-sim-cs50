#ifndef DISEASE
#define DISEASE

#include "datatypes.h"
#include <stdint.h>

void infectCitizen(citizen* cit, disease disease);
bool citizenImmune(dict_node* med_history, int med_history_size, disease disease);
void updateDisease(citizen* cit);
disease noDisease();
bool citInfected(citizen* cit);
disease randomDisease(uint8_t overall_health);
disease flu(uint8_t overall_health);
disease typhoid(uint8_t overall_health);
disease scarletFever(uint8_t overall_health);
disease pox(uint8_t overall_health);
disease plague(uint8_t overall_health);

#endif