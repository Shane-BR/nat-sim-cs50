#include "sim_time.h"
#include <GLFW/glfw3.h>

static float delta_time = 0.0f;
static float last_frame = 0.0f;

// Returns the newly calculated delta_time
float calcDeltaTime()
{
    float current_time = glfwGetTime();
    delta_time = current_time - last_frame;
    last_frame = current_time; 
    return delta_time;
}

float getDeltaTime()
{
    return delta_time;
}