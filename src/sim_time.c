#include "sim_time.h"
#include <GLFW/glfw3.h>

const unsigned int MAX_TPS = 1024;
const unsigned int START_TPS = 4;

static float delta_time = 0.0f;
static float last_frame = 0.0f;

static unsigned long ticks = 0;
static unsigned int TPS = START_TPS;

// Returns the newly calculated delta_time
float calcDeltaTime(void)
{
    float current_time = glfwGetTime();
    delta_time = current_time - last_frame;
    last_frame = current_time; 
    return delta_time;
}

float getDeltaTime(void)
{
    return delta_time;
}

void incrementTicks(void)
{
    ticks++;
}

unsigned long getTicks(void)
{
    return ticks;
}

void updateTPS()
{
    TPS = TPS*TPS > MAX_TPS ? START_TPS : TPS*TPS;
}

unsigned int getTPS()
{
    return TPS;
}