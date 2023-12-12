#include "sim_log.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

char* logs[MAX_SIM_LOGS];

void addLog(const char* str)
{
    char* log = calloc(UI_CHARACTER_LIMIT, sizeof(char));
    sprintf(log, "* %s", str);

    // Make room for new string by pushing the head out
    free(logs[MAX_SIM_LOGS-1]);
    logs[MAX_SIM_LOGS-1] = NULL;

    // Shuffle
    for(int i = MAX_SIM_LOGS-2; i >= 0; i--)
    {
        logs[i+1] = logs[i];
    }

    // Add log
    logs[0] = log;
}

char* getLog(int index)
{
    return logs[index];
}