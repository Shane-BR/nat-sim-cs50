#include "input.h"
#include "cursor.h"
#include "buttons.h"
#include "render_utils.h"
#include "datatypes.h"
#include <cglm/types.h>

// Pretty proud of this implementation. easy and effective.

typedef void (*binding_callback_func)(void);

static list_node* binding_callbacks[GLFW_KEY_LAST]; // Hash table
static vec2 mouse_pos;

void addBinding(int bind, const binding_callback_func callback);

void initBindings(void)
{
    // Cursor
    addBinding(GLFW_KEY_W, decrementCursorY);
    addBinding(GLFW_KEY_A, decrementCursorX);
    addBinding(GLFW_KEY_S, incrementCursorY);
    addBinding(GLFW_KEY_D, incrementCursorX);

    addBinding(GLFW_KEY_UP, decrementCursorY);
    addBinding(GLFW_KEY_LEFT, decrementCursorX);
    addBinding(GLFW_KEY_DOWN, incrementCursorY);
    addBinding(GLFW_KEY_RIGHT, incrementCursorX);

    addBinding(GLFW_KEY_ENTER, setCursorFocusPoint);
    addBinding(GLFW_MOUSE_BUTTON_LEFT, setCursorFocusPoint);

    // Buttons
    addBinding(GLFW_MOUSE_BUTTON_LEFT, checkButtonPressed);

}

void addBinding(int bind, const binding_callback_func callback)
{
    addLinkedListNode(&binding_callbacks[bind], callback);
}

void callBind(int bind)
{
    // Navigate each binding callback AND FUCKING CALLLL IT!!!!
    list_node* cursor = binding_callbacks[bind];
    while (cursor != NULL)
    {
        ((binding_callback_func)cursor->data)(); // *Evil laugh*
        cursor = cursor->next;
    } 
}

void processInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        callBind(key);
    }
}

void processMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_pos[0] = xpos; mouse_pos[1] = ypos;
    
    setCursorPosition(getMapPositionFromScreenPos(mouse_pos));
}

void processMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        callBind(button);
    }
}

float* getMousePosition()
{
    return mouse_pos;
}