#include "population.h"
#include "constants.h"
#include "helpers.h"
#include "disease.h"
#include "borders.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern unsigned int ticks;
extern tile map[MAP_SIZE][MAP_SIZE];

citizen* people_met[30]; 

void calcMetToday(citizen* cit, settlement* stl);
void meetPartner(citizen* cit, citizen* partner, int local_morale, int num_people_met);
void spreadDisease(citizen* carrier, citizen* suspect);
bool ofConceptionAge(int age);
void passiveHeal(citizen* cit, uint8_t overall_health);
void calcPregnancy(citizen* cit, settlement* stl);
void calcBreakup(citizen* cit);
void updatePregnancy(citizen* cit, settlement* stl);
void giveBirth(citizen* parents[2], settlement* stl, uint8_t mother_overall_health);
void oldAgeDeath(citizen* cit, settlement* stl, uint8_t overall_health);
uint8_t overallHealth(citizen* cit, uint8_t stl_local_morale);
void breakup(citizen* cit);

// Adds a bunch of random citizens between the ages of 18 and 50
void addRandomCitizens(int num, settlement* stl)
{
    int minAge = 18;
    int maxAge = 50;

    for (int i = 0; i < num; i++)
    {
        // Init new citizen
        uint8_t age = randomInt(minAge, maxAge);
        uint8_t gender = randomInt(MALE, FEMALE);
        
        citizen* c = newCitizen(age, gender, NULL);

        addCitizen(c, stl);

    }
}

// Add a citizen to current population
void addCitizen(citizen* cit, settlement* stl)
{
    citizen** tmp = stl->citizens;

    stl->citizens = realloc(stl->citizens, sizeof(citizen * ) * (stl->local_population+1));
        
    if (stl->citizens == NULL)
    {
        printf("Failed to Re-allocate memory when adding a citizen");
        stl->citizens = tmp;
        return;
    }

    stl->citizens[stl->local_population] = cit;
    cit->position = stl->position;
    stl->local_population++;
}

void removeCitizen(citizen* cit, settlement* stl)
{    
    int index = -1;

    citizen** tmp = stl->citizens;

    // Find citizen in settlement
    for (int i = 0; i < stl->local_population; i++)
    {
        if (cit == stl->citizens[i])
        {
            index = i;
            break;
        }
    }

    // Return if not found
    if (index == -1)
        return;

    // Shuffle citizen to the end of the array
    for (int i = index; i < stl->local_population-1; i++)
    {
        stl->citizens[i] = stl->citizens[i+1];
    }

    // Remove citizen
    stl->citizens[stl->local_population-1] = NULL;
    stl->local_population--;

    // Realloc memory for array
    if(stl->local_population > 0)
        stl->citizens = realloc(stl->citizens, sizeof(citizen * ) * stl->local_population); // This fucking cunt of an array is making me lose my fucking mind EDIT: nvm fixed it

    if (stl->citizens == NULL)
    {
        printf("Failed to Re-allocate memory when removing a citizen");
        stl->citizens = tmp;
        return;
    }

    // Update death_list
    addLinkedListNode(&stl->death_list, cit);

}

// Calculate the amount of people met this half-day by this citizen.
// Handle calculations that involve people that the citizen met.
void calcMetToday(citizen* cit, settlement* stl)
{
    int max_people_met = stl->local_population < 30 ? stl->local_population-1 : stl->local_population/10+30;
    int num_people_met = randomInt(0, max_people_met);

    // Meet half as many people if the cit has a disease
    num_people_met = cit->disease.active ? num_people_met/2 : num_people_met;

    // Doesn't account for getting the same person twice... But I don't think I care.
    // TODO maybe fix this later I don't fucking know
    for (int i = 0; i < num_people_met; i++)
    {
        citizen* rand_c = stl->citizens[randomInt(0, stl->local_population-1)];
        
        if (rand_c == cit) 
        {
            i--;
            continue;
        }

        // FIXME NO GAYS
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
    uint8_t overall_health = overallHealth(cit, stl->local_morale);
    
    if (citInfected(cit)) // NOT cit->disease.active because this function also updates the incubation period // TODO Change to something more readable later
    {
        updateDisease(cit);

    }
    // Randommly insert diseases into the settlement
    // TODO redo this so it's A LOT less random.
    else if (runProbability((float)(UINT8_MAX-overall_health) / UINT8_MAX / 1000))
    {
        infectCitizen(cit, randomDisease(overall_health));
        // if(cit->disease.type != NULL && stl->nation == 1)
        //     printf("RAND INFECT CIT AGED %i\n", cit->age);
    }

    // Check if dead
    if (cit->health <= 0)
    {
        breakup(cit);
        removeCitizen(cit, stl);
        return;
    }

    // Elderly death calculations
    oldAgeDeath(cit, stl, overall_health);

    // Update pregnancy if applicable
    if (cit->pregnant)
        updatePregnancy(cit, stl);

    passiveHeal(cit, overall_health);
}

void spreadDisease(citizen* carrier, citizen* suspect)
{
    float max_inf = 10.0f; // MAX PERCENTAGE
    float infectivity = ((float)carrier->disease.infectivity_rate / UINT8_MAX)*max_inf;
    if (runProbability(infectivity))
    {
        infectCitizen(suspect, carrier->disease);
        // if(suspect->disease.type != NULL)
        //     printf("Cit age %i just infected Cit aged %i   DISEASE: %s\n", carrier->age, suspect->age, carrier->disease.type);
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
        cit->disease.immunity_period <= 0
        ;

    int min_rand_div = 90;
    int max_rand_div = 120;

    if (allow_heal)
        cit->health += ((float)overall_health / randomInt(min_rand_div, max_rand_div));
}

// Updates stats of citizens in relationships
void updateRelationship(citizen* cit, settlement* stl)
{
    if (cit->partner != NULL) 
    {
        // Update days in relationship
        if (ticks % TICKS_PER_DAY == 0)
            cit->days_in_relationship++;

        calcPregnancy(cit, stl);
        calcBreakup(cit);
    }

}

void calcPregnancy(citizen* cit, settlement* stl)
{
    int calc_period = 1461*TICKS_PER_DAY; // DAYS

    if (cit->gender != FEMALE)
        return;

    if (cit->partner == NULL) // TODO handle affairs MAYYYBE
        return;

    if (cit->children != NULL && cit->children[cit->child_num-1]->age < 1) // Already has a child younger than one
        return;

    if (cit->pregnant == false)
    {
        citizen* partner = cit->partner;

        // Chance of the current females partner being the father of this child
        int partner_father_chance;
        int min_pf_chance = 80; // PERCENTAGE

        // (Stats that factor into pregnancy rate) / 255*n (n = num of factors)

        // Min health OF BOTH PARTNERS for pregnancy
        int min_health = 50;

        // Max infliction OF BOTH PARTNERS severity for pregnancy
        int max_inf_sev = 150;

        if ((cit->health < min_health || partner->health < min_health) || 
        (cit->disease.severity > max_inf_sev || partner->disease.severity > max_inf_sev))
            return;

        // Average stats between both partners
        uint8_t avg_health = (cit->health+partner->health)/2;
        uint8_t avg_inf_sev = (UINT8_MAX - (cit->disease.severity+partner->disease.severity)/2);
        uint8_t avg_age = (cit->age + partner->age)/2;

        // Max days before dur_clamped_1byte is maxed out (1825 = 5 years)
        int max_dur = 1825;

        // Used for calculating rate factors
        uint8_t dur_clamped_1byte = (uint8_t)(cit->days_in_relationship/max_dur) * UINT8_MAX;

        // Max children before child_factor is maxed out
        uint8_t max_children = 4;
        uint8_t child_factor = UINT8_MAX - (uint8_t)(cit->child_num/max_children)*UINT8_MAX;
    
        int factors = (avg_health + avg_inf_sev + dur_clamped_1byte + getAgeHealthCurve(avg_age) + cit->hunger + child_factor);
        float preg_rate = ((float)factors / (UINT8_MAX*6) * 100) / calc_period; // PERCENTAGE

        if(runProbability(preg_rate)) 
        {
            citizen* parents[2] = {cit, partner};

            cit->pregnant = true;

            // Create new child for later reference
            cit->unborn_child = newCitizen(0, randomInt(MALE, FEMALE), parents);
        }
    }
}

void calcBreakup(citizen* cit)
{
    int tick_year = 365*TICKS_PER_DAY;

    float probability; // PERCENTAGE
        
    if (cit->age <= 20)
    {
        probability = 70./tick_year;
    }
    else if (cit->age <= 28)
    {
        probability = 50./tick_year;
    }
    else if (cit->age > 28)
    {
        probability = 20./tick_year;
    }

    // if with child or child under 2 take away 0.25 for each child already over 2 - 2x probability
    uint8_t max_child_age_high_prob = 2;
    if ((cit->pregnant || cit->partner->pregnant) || (cit->children != NULL && cit->children[cit->child_num-1]->age < max_child_age_high_prob)) 
    {
        // Work our way down from the end of the array until we see a child above max age
        // Fewer steps to the same sum
        int children_over_age_count = 0;
        for (int i = cit->child_num-1; i > 0; i--)
        {
            if (cit->children[i]->age > max_child_age_high_prob) 
            {
                break;
            }

            children_over_age_count = cit->child_num - (i+1);
        }

        float mltp = children_over_age_count > 4 ? 1 : 2 - (children_over_age_count * 0.25f);
        probability = probability * mltp;
        // TODO handle affairs
    }

    if (runProbability(probability))
    {
        breakup(cit);
    }
}

void updatePregnancy(citizen* cit, settlement* stl)
{
    if (ticks % TICKS_PER_DAY != 0)
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
    addCitizen(child, stl);

    parents[0]->unborn_child = NULL;

    uint8_t survivability = map[stl->position.y][stl->position.x].survivability;
    float comb_stats = ((float)mother_overall_health + survivability) / (UINT8_MAX*2);
    float max_rate = 20.0f; // PERCENTAGE

    // Handle chance of stillbirth
    float stillbirth_rate = max_rate - comb_stats * max_rate;

    // Kill the child if probability accepts
    if (runProbability(stillbirth_rate)) 
        removeCitizen(child, stl);

    // Handle chance of the mother dying upon birth of the child
    max_rate = 5.0f; // PERCENTAGE
    float dib_rate = max_rate - comb_stats * max_rate;

    // Kill the mother if probability accepts
    if (runProbability(dib_rate)) 
        removeCitizen(parents[0], stl);
}

void oldAgeDeath(citizen* cit, settlement* stl, uint8_t overall_health)
{
    if (cit->age > 60)
    {
        // Probability of random death this year.  Increases by 20% with every year

        if (ticks*TICKS_PER_DAY % 365 != 0)
            return;

        float growth = 0.2f;
        float death_prob = (1-(float)overall_health/UINT8_MAX) * pow((1 + growth), cit->age-60);
        if (runProbability(death_prob)) 
            removeCitizen(cit, stl);
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

bool ofConceptionAge(int age)
{
    return age >= MIN_CONCEPTION_AGE && age <= MAX_CONCEPTION_AGE;
}

uint8_t overallHealth(citizen* cit, uint8_t stl_local_morale)
{
    // Weighted average for working out overall_health with age being the biggest factor
    float age_weight = 0.5f;
    float conditions_weight = 0.3f;
    float hunger_weight = 0.1f; // Hunger is only this low because it will start killing the cit when too low anyway
    float morale_weight = 0.1f;

    uint8_t overall_health = 
    (
        getAgeHealthCurve(cit->age)*age_weight + 
        cit->living_conditions*conditions_weight + 
        cit->hunger*hunger_weight + 
        stl_local_morale*morale_weight
    );

    return overall_health;
}

void breakup(citizen* cit)
{
    if (cit->partner != NULL) {
        cit->partner->days_in_relationship = 0;
        cit->partner->partner = NULL; // What the actual fuck
    } 

    cit->days_in_relationship = 0;
    cit->partner = NULL;
}