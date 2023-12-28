#include "helpers.h"
#include "disease.h"
#include "constants.h"
#include "population.h"
#include "sim_time.h"
#include <stdint.h>
#include <math.h>

// Constants to base the infliction severity and disease infectivity off of (out of 0-255)
static const int LOW = 50;
static const int MID = 75;
static const int HIGH = 150;
static const int EXTREME = 200;

static int DAMAGE_DIVISOR_MIN = 10, DAMAGE_DIVISOR_MAX = 20;


// Infect citizen if he does not already have immunity.
void infectCitizen(citizen* cit, disease disease)
{
    // Check for immunity
    if (!citizenImmune(cit->medical_history, cit->med_history_size, disease)) 
    {
        // Infect
        cit->disease = disease;
        addDictNode(&cit->medical_history, &cit->med_history_size, disease.type, getTicks());
    }
}

bool citizenImmune(dict_node* med_history, int med_history_size, disease disease)
{
    const int IMMUNITY_PERIOD = disease.immunity_period*TICKS_PER_DAY;
    int i = dictFind(med_history, med_history_size, disease.type);

    if (i != -1)
    {
        if ((getTicks()-med_history[i].value) >= IMMUNITY_PERIOD)
        {
            return false;
        }
        else 
        {
            return true;
        }
    }

    return false;
}

void updateDisease(citizen* cit)
{
    if (getTicks() % TICKS_PER_DAY == 0) 
    {
        // Is disease active?
        if (cit->disease.active) 
        {
            cit->disease.days_to_recover--;

            damageCitizen(cit, cit->disease.damage, DISEASE);

            if (cit->disease.days_to_recover <= 0)
            {
                cit->disease = noDisease();
            }
        }

        // Or still incubating?
        else if (cit->disease.type != NULL) 
        {
            cit->disease.incubation_period--;

            if (cit->disease.incubation_period <= 0)
            {
                cit->disease.active = true;
            } 
        }
    }
}

// Returns true if citizen is infected with any disease or has an incubation period.
// Use cit.disease.active for active disease only.
bool citInfected(citizen* cit)
{
    return cit->disease.active || cit->disease.incubation_period > 0;
}

disease noDisease()
{
    disease out;
    out.active = false;
    out.days_to_recover = 0;
    out.damage = 0;
    out.severity = 0;
    out.infectivity_rate = 0;
    out.type = NULL;
    out.immunity_period = 0;
    out.incubation_period = 0;
    return out;
}

// Can be dirty because we are getting rid of this function in later release
disease randomDisease(uint8_t overall_health)
{
    disease d_flu = flu(overall_health);
    disease d_typhoid = typhoid(overall_health);
    disease d_scarlet_fever = scarletFever(overall_health);
    disease d_pox = pox(overall_health);
    disease d_plague = plague(overall_health);

    // yucko
    if (runProbability(((float)d_flu.infectivity_rate / UINT8_MAX) * 100))
    {
        return d_flu;
    }
    else if (runProbability(((float)d_typhoid.infectivity_rate / UINT8_MAX) * 100))
    {
        return d_typhoid;
    }
    else if(runProbability(((float)d_scarlet_fever.infectivity_rate / UINT8_MAX) * 100))
    {
        return d_scarlet_fever;
    }
    else if (runProbability(((float)d_pox.infectivity_rate / UINT8_MAX) * 100))
    {
        return d_pox;
    }
    else if (runProbability(((float)d_plague.infectivity_rate / UINT8_MAX) * 100)){
        return d_plague;
    }
    else 
    {
        return d_flu;
    }
}

disease flu(uint8_t overall_health)
{
    disease out;

    // MIN/MAX days infected
    const int MIN_DAYS = 5, MAX_DAYS = 21;

    out.active = false; // FALSE until incubation period has passed

    out.severity = randomInt(LOW, MID) + (UINT8_MAX - overall_health);
    out.days_to_recover = powerLawRandomInt(MIN_DAYS, MAX_DAYS, (double)out.severity / 100);
    out.infectivity_rate = HIGH;
    out.immunity_period = 245;
    out.damage = (double)out.severity/randomInt(DAMAGE_DIVISOR_MIN, DAMAGE_DIVISOR_MAX);
    out.type = "flu";
    out.incubation_period = randomInt(1, 8);

    return out;
}

disease typhoid(uint8_t overall_health)
{
    disease out;

    // MIN/MAX days infected
    const int MIN_DAYS = 7, MAX_DAYS = 35;

    out.active = false;

    out.severity = randomInt(MID, HIGH) + (UINT8_MAX - overall_health);
    out.days_to_recover = powerLawRandomInt(MIN_DAYS, MAX_DAYS, (double)out.severity / 100);
    out.infectivity_rate = LOW;
    out.immunity_period = 182;
    out.damage = (double)out.severity/randomInt(DAMAGE_DIVISOR_MIN, DAMAGE_DIVISOR_MAX);
    out.type = "typhoid";
    out.incubation_period = randomInt(6, 30);

    return out;
}

disease scarletFever(uint8_t overall_health)
{
    disease out;

    // MIN/MAX days infected
    const int MIN_DAYS = 5, MAX_DAYS = 12;

    out.active = false;

    out.severity = randomInt(MID, HIGH) + (UINT8_MAX - overall_health);
    out.days_to_recover = powerLawRandomInt(MIN_DAYS, MAX_DAYS, (double)out.severity / 100);
    out.infectivity_rate = MID;
    out.immunity_period = UINT16_MAX; // Natural immunity is permanent
    out.damage = (double)out.severity/randomInt(DAMAGE_DIVISOR_MIN, DAMAGE_DIVISOR_MAX);
    out.type = "scarlet_fever";
    out.incubation_period = randomInt(2, 5);

    return out;
}

disease pox(uint8_t overall_health)
{
    disease out;

    // MIN/MAX days infected
    const int MIN_DAYS = 7, MAX_DAYS = 35;

    out.active = false;

    out.severity = randomInt(MID, EXTREME) + (UINT8_MAX - overall_health);
    out.days_to_recover = powerLawRandomInt(MIN_DAYS, MAX_DAYS, (double)out.severity / 100);
    out.infectivity_rate = MID;
    out.immunity_period = UINT16_MAX; // Natural immunity is permanent
    out.damage = (double)out.severity/randomInt(DAMAGE_DIVISOR_MIN, DAMAGE_DIVISOR_MAX);
    out.type = "pox";
    out.incubation_period = randomInt(10, 14);

    return out;
}

disease plague(uint8_t overall_health)
{
    disease out;

    // MIN/MAX days infected
    const int MIN_DAYS = 7, MAX_DAYS = 35;

    out.active = false;

    out.severity = randomInt(HIGH, EXTREME) + (UINT8_MAX - overall_health);
    out.days_to_recover = powerLawRandomInt(MIN_DAYS, MAX_DAYS, (double)out.severity / 100);
    out.infectivity_rate = HIGH;
    out.immunity_period = 365;
    out.damage = (double)out.severity/randomInt(DAMAGE_DIVISOR_MIN, DAMAGE_DIVISOR_MAX);
    out.type = "plague";
    out.incubation_period = randomInt(2, 8);

    return out;
}