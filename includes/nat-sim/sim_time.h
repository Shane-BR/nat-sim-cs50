#ifndef NS_TIME
#define NS_TIME

float calcDeltaTime(void);
float getDeltaTime(void);
void incrementTicks(void);
unsigned long getTicks(void);
void updateTPS(void);
void setTPS(const unsigned int new_val);
unsigned int getTPS(void);
float getFPS(void);

#endif