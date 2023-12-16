#define _CRT_SECURE_NO_WARNINGS
#include "sim_log.h"
#include "constants.h"
#include "helpers.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static char** logs;
static unsigned long logs_size = 0; 
static unsigned long logs_max = 512; 

void addLog(const char* str)
{

    if (logs_size >= ULONG_MAX)
        return;

    if (logs_size <= 0)
        logs = calloc(logs_max, sizeof(char*));

    char* log = calloc(UI_CHARACTER_LIMIT, sizeof(char));
    sprintf(log, "* %s", str);

    // Relloc
    if (++logs_size > logs_max)
        logs = realloc(logs, sizeof(char*)*clamp(logs_max *= 2, 0, ULONG_MAX));

    // Shuffle
    for(int i = logs_size-2; i >= 0; i--)
    {
        logs[i+1] = logs[i];
    }

    // Add log

    logs[0] = log;
}

char* getLog(int index)
{
    if (index >= logs_size || index < 0)
        return NULL;

    return logs[index];
}

unsigned long getLogsSize()
{
    return logs_size;
}

void printToLogsFile()
{
    char date[128];
    char* folder_name = "logs";
    time_t now = time(NULL);
    strftime(date, sizeof(date)-1, "%d-%m-%Y %I-%M-%S %p", localtime(&now));
    char filename[256];

    sprintf(filename, "../%s/log_%s.txt", folder_name, date);
    
    char command[128];
    sprintf(command, "mkdir ..\\%s", folder_name);

    system(command); // Messy but whatever. Fix later maybe
    FILE* fptr = fopen(filename, "w+");

    if (fptr == NULL)
        return;

    for (int i = 0; i < logs_size; i++)
    {
        fprintf(fptr, "%s\n", logs[i]);
        free(logs[i]);
    }

    fclose(fptr);
    free(logs);
}