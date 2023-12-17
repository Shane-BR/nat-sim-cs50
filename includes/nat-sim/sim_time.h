#ifndef NS_TIME
#define NS_TIME

float calcDeltaTime(void);
float getDeltaTime(void);
void incrementTicks(void);
unsigned long getTicks(void);
void updateTPS();
unsigned int getTPS();

#endif