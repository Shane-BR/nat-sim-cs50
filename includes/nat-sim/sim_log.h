#ifndef NS_SIM_LOG
#define NS_SIM_LOG

void addLog(const char* str);
char* getLog(int index);
unsigned long getLogsSize();
void printToLogsFile();
#endif