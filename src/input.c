#include "input.h"
#include "cursor.h"
#include "render.h"
#include <cglm/types.h>

// TODO refine in later release
void processInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        decrementCursorY();
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        incrementCursorY();
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        decrementCursorX();
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        incrementCursorX();
    }

    if (key == GLFW_KEY_ENTER)
    {
        setCursorPressedState(action);
    }
}

void processMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    vec2 pos = {xpos, ypos};
    
    setCursorPosition(getMapPositionFromScreenPos(pos));
}

void processMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        setCursorPressedState(action);  
}