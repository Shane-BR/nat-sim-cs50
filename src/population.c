#define _CRT_SECURE_NO_WARNINGS
#include "population.h"
#include "constants.h"
#include "helpers.h"
#include "disease.h"
#include "borders.h"
#include "settlements.h"
#include "sim_log.h"
#include "sim_time.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// FIXME Magic Numbers (YUP)

extern tile map[MAP_SIZE][MAP_SIZE];

static citizen* people_met[30]; 

void calcMetToday(citizen* cit, settlement* stl);
void meetPartner(citizen* cit, citizen* partner, int local_morale, int num_people_met);
void spreadDisease(citizen* carrier, citizen* suspect);
bool ofConceptionAge(const int age);
void passiveHeal(citizen* cit, uint8_t overall_health);
void calcPregnancy(citizen* cit, settlement* stl);
void calcBreakup(citizen* cit);
void updatePregnancy(citizen* cit, settlement* stl);
void giveBirth(citizen* parents[2], settlement* stl, uint8_t mother_overall_health);
void citizenDie(citizen* cit, settlement* stl, CitDamageSource cause);
void oldAgeDeath(citizen* cit, settlement* stl, uint8_t overall_health);
uint8_t overallHealth(citizen* cit, uint8_t stl_local_morale);
void breakup(citizen* cit);
void logCitizenDeath(CitDamageSource cause, const char* disease_name, const unsigned int citizen_age);

// Adds a bunch of random citizens between the ages of 18 and 50
void addRandomCitizens(int num, position init_pos, citizen*** arr, unsigned int* cur_size, unsigned int* mem_block_size)
{
    int minAge = 18;
    int maxAge = 50;

    for (int i = 0; i < num; i++)
    {
        // Init new citizen
        uint8_t age = randomInt(minAge, maxAge);
        uint8_t gender = randomInt(MALE, FEMALE);
        
        citizen* c = newCitizen(age, gender, NULL, init_pos);

        addToDynamicPointerArray((void***)arr, cur_size, c, mem_block_size);

    }
}

// Returns false if no citizen found
bool removeCitizen(citizen* cit, settlement* stl)
{    
    bool out = removeFromDynamicPointerArray((void***)&stl->citizens, &stl->local_population, cit, false);
    removeCitFromBorder(cit, stl);
    return out;
}

// Calculate the amount of people met this half-day by this citizen.
// Handle calculations that involve people that the citizen met.
void calcMetToday(citizen* cit, settlement* stl)
{
    const int MAX_PEOPLE_MET = stl->local_population < 30 ? stl->local_population-1 : stl->local_population/12+30;
    
    int num_people_met = randomInt(0, MAX_PEOPLE_MET);

    // Meet half as many people if the cit has a disease
    num_people_met = cit->disease.active ? num_people_met/2 : num_people_met;

    // Doesn't account for getting the same person twice... But I don't think I care.
    // TODO maybe fix this later I don't know
    for (int i = 0; i < num_people_met; i++)
    {
        citizen* rand_c = stl->citizens[randomInt(0, stl->local_population-1)];
        
        if (rand_c == cit) 
        {
            i--;
            continue;
        }

        if (cit->partner == NULL && rand_c->partner == NULL && rand_c->gender != cit->gender && cit->age > 14)
        {
            meetPartner(cit, rand_c, stl->local_morale, num_people_met);
        }

        if (cit->disease.active && !citInfected(rand_c))
        {
            spreadDisease(cit, rand_c);
        }
    }
}

// Further flesh out this function later
void healthCheck(citizen* cit, settlement* stl)
{
    const uint8_t OVERALL_HEALTH = overallHealth(cit, stl->local_morale);
    const float DIVISOR = 1000;

    int chance_to_eat_if_hungry = stl->food > stl->local_population ? 100 :
        (((float)stl->food / stl->local_population * UINT8_MAX) +
        (float)cit->hunger / (float)(UINT_MAX*2)) * 100; // Ugly as hell but it does the job

    if (citInfected(cit)) // NOT cit->disease.active because this function also updates the incubation period // TODO Change to something more readable later
    {
        updateDisease(cit);
    }
    // Randommly insert diseases into the settlement
    // TODO redo this so it's A LOT less random.
    else if (runProbability((float)(UINT8_MAX-OVERALL_HEALTH) / UINT8_MAX / DIVISOR))
    {
        infectCitizen(cit, randomDisease(OVERALL_HEALTH));
    }

    // Check if dead
    if (cit->health <= 0)
    {
        breakup(cit);
        citizenDie(cit, stl, cit->last_damage_source);
        return;
    }

    // Increase hunger
    cit->hunger = clamp(cit->hunger + HUNGER_INCREASE_RATE, 0, UINT8_MAX);

    // Eat if hungry
    if (stl->food > 1 && cit->hunger > FOOD_TO_HUNGER && runProbability(chance_to_eat_if_hungry))
    {
        cit->hunger = clamp(cit->hunger -= FOOD_TO_HUNGER, 0, UINT8_MAX);
        stl->food--;
    }

    // Update hunger
    // Elderly death calculations
    oldAgeDeath(cit, stl, OVERALL_HEALTH);

    // Update pregnancy if applicable
    if (cit->pregnant)
        updatePregnancy(cit, stl);

    // Check starvation
    if (cit->hunger >= UINT8_MAX) 
    {
        damageCitizen(cit, UINT8_MAX / (STARVE_PERIOD / 2), STARVATION);
    }

    // Can still work?
    if (!canCitizenWork(cit))
    {
        if(cit->citizen_class == GATHERER) removeCitFromBorder(cit, stl);

        if(cit->citizen_class != NONE) cit->citizen_class = NONE;
    }

    passiveHeal(cit, OVERALL_HEALTH);
}

void spreadDisease(citizen* carrier, citizen* suspect)
{
    const float MAX_INFECTIVITY_PERCENT = 2.0f; // MAX PERCENTAGE
    float infectivity_percent = ((float)carrier->disease.infectivity_rate / UINT8_MAX)*MAX_INFECTIVITY_PERCENT;
    if (runProbability(infectivity_percent))
    {
        infectCitizen(suspect, carrier->disease);
    }
}

// Run checks to check propability of this cit becoming the partner of the other cit
void meetPartner(citizen* cit, citizen* partner, int local_morale, int num_people_met)
{
    // Work out max age diff
    int max_age_dif;
        
    if (cit->age <= 20)
        max_age_dif = 3;
    else if (cit->age <= 28)
        max_age_dif = 5;
    else if (cit->age > 28)
        max_age_dif = 7;
        
    if (abs(cit->age - partner->age) > max_age_dif)
        return;

    float chance_of_relationship = ((float)local_morale / UINT8_MAX) / num_people_met; // PERCENTAGE

    if (runProbability(chance_of_relationship)) 
    {
        // Set partner
        cit->partner = partner;
        partner->partner = cit;
    }    
}

void passiveHeal(citizen* cit, uint8_t overall_health)
{
    bool allow_heal =
        cit->health < UINT8_MAX && 
        overall_health > 50 &&
        !cit->disease.active &&
        cit->disease.immunity_period <= 0 &&
        cit->hunger < 50;
        ;

    const int MIN_RAND_DIV = 90;
    const int MAX_RAND_DIV = 120;

    if (allow_heal)
        cit->health += ((float)overall_health / randomInt(MIN_RAND_DIV, MAX_RAND_DIV));
}

void damageCitizen(citizen *cit, uint8_t amt, CitDamageSource cause)
{
    cit->last_damage_source = cause;
    cit->health -= amt;
}

// Updates stats of citizens in relationships
void updateRelationship(citizen* cit, settlement* stl)
{
    if (cit->partner != NULL) 
    {
        // Update days in relationship
        if (getTicks() % TICKS_PER_DAY == 0)
            cit->days_in_relationship++;

        calcPregnancy(cit, stl);
        calcBreakup(cit);
    }

}

void calcPregnancy(citizen* cit, settlement* stl)
{
    const int CALC_PERIOD = (365*TICKS_PER_DAY)*2; // DAYS

    if (cit->gender != FEMALE)
        return;

    if (cit->partner == NULL)
        return;

    if (cit->children != NULL && cit->children[cit->child_num-1]->age < 1) // Already has a child younger than one
        return;

    if (cit->pregnant == false)
    {
        citizen* partner = cit->partner;

        // Chance of the current females partner being the father of this child
        int partner_father_chance; // FIXME not used at the momento
        int min_pf_chance = 80; // PERCENTAGE

        // (Stats that factor into pregnancy rate) / 255*n (n = num of factors)

        // Min health OF BOTH PARTNERS for pregnancy
        const int MIN_HEALTH = 50;

        // Max infliction OF BOTH PARTNERS severity for pregnancy
        const int MAX_INFLICTION_SEVERITY = 150;

        if ((cit->health < MIN_HEALTH || partner->health < MIN_HEALTH) || 
        ((cit->disease.active && cit->disease.severity > MAX_INFLICTION_SEVERITY) 
        || (cit->disease.active && partner->disease.severity > MAX_INFLICTION_SEVERITY)))
            return;

        // Average stats between both partners
        const uint8_t AVG_HEALTH = (cit->health+partner->health)/2;
        const uint8_t AVG_INF_SEV = (UINT8_MAX - (cit->disease.severity+partner->disease.severity)/2);
        const uint8_t AVG_AGE = (cit->age + partner->age)/2;

        // Max days before dur_clamped_1byte is maxed out (1825 = 5 years)
        const int MAX_DURATION = 1825;

        // Used for calculating rate factors
        const uint8_t MAX_DUR_NORM = (uint8_t)(cit->days_in_relationship/MAX_DURATION) * UINT8_MAX;

        // Max children before child_factor is maxed out
        const uint8_t MAX_CHILDREN = 4;
        const uint8_t CHILD_FACTOR = UINT8_MAX - (uint8_t)(cit->child_num/MAX_CHILDREN)*UINT8_MAX;
    
        const int FACTORS = (AVG_HEALTH + AVG_INF_SEV + MAX_DUR_NORM + getAgeHealthCurve(AVG_AGE) + CHILD_FACTOR);
        const float PREGNANCY_RATE = ((float)FACTORS / (UINT8_MAX*5) * 100) / CALC_PERIOD; // PERCENTAGE

        if(runProbability(PREGNANCY_RATE)) 
        {
            citizen* parents[2] = {cit, partner};

            cit->pregnant = true;

            // Create new child for later reference
            cit->unborn_child = newCitizen(0, randomInt(MALE, FEMALE), parents, stl->position);
        }
    }
}

void calcBreakup(citizen* cit)
{
    const int TICK_YEAR = 365*TICKS_PER_DAY;

    float probability; // PERCENTAGE
        
    if (cit->age <= 20)
    {
        probability = 70./TICK_YEAR;
    }
    else if (cit->age <= 28)
    {
        probability = 50./TICK_YEAR;
    }
    else if (cit->age > 28)
    {
        probability = 20./TICK_YEAR;
    }

    // if with child or, child under 2, take away 0.25 for each child already over 2 - 2x probability
    const uint8_t MAX_CHILDREN_FACTOR = 2;
    if ((cit->pregnant || cit->partner->pregnant) || (cit->children != NULL && cit->children[cit->child_num-1]->age < MAX_CHILDREN_FACTOR)) 
    {
        // Work our way down from the end of the array until we see a child above max age
        // Fewer steps to the same sum
        int children_over_age_count = 0;
        for (int i = cit->child_num-1; i > 0; i--)
        {
            if (cit->children[i]->age > MAX_CHILDREN_FACTOR) 
            {
                break;
            }

            children_over_age_count = cit->child_num - (i+1);
        }

        const float FACTOR_MULTIPLIER = children_over_age_count > 4 ? 1 : 2 - (children_over_age_count * 0.25f);
        probability = probability * FACTOR_MULTIPLIER;
        // TODO handle affairs
    }

    if (runProbability(probability))
    {
        breakup(cit);
    }
}

void updatePregnancy(citizen* cit, settlement* stl)
{
    if (getTicks() % TICKS_PER_DAY != 0)
        return;

    cit->days_pregnant++;

    uint8_t overall_health = overallHealth(cit, stl->local_morale);

    // min/max weeks until birth
    int min_weeks = 35, max_weeks = 45;

    // Max days before miscarriage is no longer a risk
    int mc_risk_period = 105;
    // Randomly generated every 2 ticks... who fucking cares
    int birth_time = randomInt(min_weeks*7, max_weeks*7);

    if (cit->days_pregnant < mc_risk_period)
    {
        float max_mc_risk = 20.0f; // PERCENTAGE

        // MAX = 30% over first 15 weeks of pregnancy
        float mc_rate = (max_mc_risk - ((float)overall_health/UINT8_MAX)*max_mc_risk) / mc_risk_period;

        if (runProbability(mc_rate))
        {
            // Misscariage
            
            // Reset stats
            cit->pregnant = false;
            cit->days_pregnant = 0;

            // Kill
            free(cit->unborn_child);
            cit->unborn_child = NULL;

            // TODO Lower morale of stl
            
        }
    }

    if (cit->days_pregnant >= birth_time)
    {
        citizen* parents[2] = {cit, cit->partner};
        giveBirth(parents, stl, overallHealth(cit, stl->local_morale));
        cit->pregnant = false;
        cit->days_pregnant = 0;
    }
}

// Births new citizen from 2 parents.
// The mother should be the first element.
void giveBirth(citizen* parents[2], settlement* stl, uint8_t mother_overall_health)
{

    // If the cit is pregnant then we can assume that the last child in the list is not yet born.
    citizen* child = parents[0]->unborn_child;

    // Add citizen to this settlements population
    addToDynamicPointerArray((void***)&stl->citizens, &stl->local_population, child, &stl->population_capacity);

    parents[0]->unborn_child = NULL;

    const uint8_t SURVIVABILITY = map[stl->position.y][stl->position.x].survivability;
    float COMBINED_STATS = ((float)mother_overall_health + SURVIVABILITY) / (UINT8_MAX*2);
    float max_rate = 20.0f; // PERCENTAGE

    // Handle chance of stillbirth
    const float STILLBIRTH_RATE = max_rate - COMBINED_STATS * max_rate;

    // Kill the child if probability accepts
    if (runProbability(STILLBIRTH_RATE)) 
        citizenDie(child, stl, STILLBIRTH);

    // Handle chance of the mother dying upon birth of the child
    max_rate = 5.0f; // PERCENTAGE
    const float CHILDBIRTH_DEATH_RATE = max_rate - COMBINED_STATS * max_rate;

    // Kill the mother if probability accepts
    if (runProbability(CHILDBIRTH_DEATH_RATE)) 
        citizenDie(parents[0], stl, CHILDBIRTH);
}

void citizenDie(citizen* cit, settlement* stl, CitDamageSource cause)
{
    // Is DISEASE cause still ative

    char* disease_name = cit->disease.active ? cit->disease.type : NULL;

    if (cause == DISEASE && !cit->disease.active)
    {
        // Get last disease in medical history
        disease_name = (char*)cit->medical_history[cit->med_history_size-1].key;
    }

    if(removeCitizen(cit, stl))
    {
        // Update death_list
        addLinkedListNode(&getNation(stl->nation)->death_list, cit);
        logCitizenDeath(cause, disease_name, cit->age);
    }
}

void oldAgeDeath(citizen* cit, settlement* stl, uint8_t overall_health)
{
    if (cit->age > 60)
    {
        // Probability of random death this year.  Increases by 20% with every year

        if (getTicks()*TICKS_PER_DAY % 365 != 0)
            return;

        float growth = 0.2f;
        float death_prob = (1-(float)overall_health/UINT8_MAX) * pow((1 + growth), cit->age-60);
        if (runProbability(death_prob)) 
            citizenDie(cit, stl, NATURAL);
    }
}

/*
0-255.
Anything over 80 or under 5 will return the minimum.
Not used for old age death.
*/
uint8_t getAgeHealthCurve(uint8_t age)
{
    int peak_age = 30;
    int max_calc_age = 80;
    int min_calc_age = 5;

    float min = UINT8_MAX * 0.05; // 5% minimum

    if (age > max_calc_age || age < min_calc_age)
        return (uint8_t)min;

    float growth_rate = (float)(UINT8_MAX - min) / peak_age;
    float decline_rate = (float)(UINT8_MAX - min) / (max_calc_age - peak_age);

    return age <= peak_age ? age * growth_rate + min : UINT8_MAX - (age - peak_age) * decline_rate;
}

bool ofConceptionAge(const int age)
{
    return age >= MIN_CONCEPTION_AGE && age <= MAX_CONCEPTION_AGE;
}

uint8_t overallHealth(citizen* cit, uint8_t stl_local_morale)
{
    // Weighted average for working out overall_health with age being the biggest factor
    float age_weight = 0.5f;
    float conditions_weight = 0.3f;
    float morale_weight = 0.1f;

    uint8_t overall_health = 
    (
        getAgeHealthCurve(cit->age)*age_weight + 
        cit->living_conditions*conditions_weight + 
        stl_local_morale*morale_weight
    );

    return overall_health;
}

void breakup(citizen* cit)
{
    if (cit->partner != NULL) 
    {
        cit->partner->days_in_relationship = 0;
        cit->partner->partner = NULL; // What the actual fuck
    } 

    cit->days_in_relationship = 0;
    cit->partner = NULL;
}



bool canCitizenWork(citizen* cit)
{
    return cit->disease.severity < DEBILITATING_DISEASE_SEVERITY && (cit->age >= MIN_WORKING_AGE || cit->age <= MAX_WORKING_AGE);
}

// Leave char* null if no disease
void logCitizenDeath(CitDamageSource cause, const char* disease_name, const unsigned int citizen_age)
{
    char cause_str[128] = {'\0'};
    switch (cause) 
    {
        case NATURAL:
            strcpy(cause_str, "from natural causes");
            break;
        case DISEASE:
            strcpy(cause_str, "from disease: \0");
            strcat(cause_str, disease_name);
            break;
        case STARVATION:
            strcpy(cause_str, "from starvation");
            break;
        case INJURY:
            strcpy(cause_str, "from their injuries");
            break;
        case CHILDBIRTH:
            strcpy(cause_str, "in childbirth");
            break;
        case STILLBIRTH:
            strcpy(cause_str, "stillborn");
            break;
    }

    char death_log[128];
    sprintf(death_log, "Citizen died aged %i, %s.", citizen_age, cause_str);
    addLog(death_log);
}