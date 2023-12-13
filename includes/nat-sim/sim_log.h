#ifndef NS_SIM_LOG
#define NS_SIM_LOG

#define MAX_SIM_LOGS 8

void addLog(const char* str);
char* getLog(int index);

#endif