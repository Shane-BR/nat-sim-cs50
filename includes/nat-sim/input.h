#ifndef INPUT
#define INPUT

#include <GLFW/glfw3.h>

void processInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void processMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

#endif