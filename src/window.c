#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "window.h"
#include "cglm/cam.h"
#include "sprite_renderer.h"
#include "shaders.h"
#include "render.h"
#include "input.h"
#include "text_renderer.h"
#include "render_utils.h"

#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <cglm/types.h>

#define START_RES_W 1024
#define START_RES_H 768

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void errorCallback(int code, const char* error);
GLFWwindow* window = NULL;

int windowWidth = START_RES_W;
int windowHeight = START_RES_H;

mat4 projection;

void setProjectionMatrix(void);

void initWindow(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "NatSim", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        exit(3);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        exit(4);
    }

    glViewport(0, 0, windowWidth, windowHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, processInputCallback);
    glfwSetCursorPosCallback(window, processMousePositionCallback);
    glfwSetMouseButtonCallback(window, processMouseButtonCallback);
    glfwSetErrorCallback(errorCallback);
    glm_mat4_identity(projection);
    initSpriteRenderer();
    loadFont();
    setProjectionMatrix();
}

int shouldWindowClose(void)
{
    return glfwWindowShouldClose(window);
}

void updateWindow(void)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void terminateWindow(void)
{
    glfwTerminate();
}

void setProjectionMatrix(void)
{

    glm_ortho(0.0f, windowWidth, windowHeight, 0.0f, -1.0f, 1.0f, projection);
    //glm_ortho_default_rh_no((float)windowWidth / windowHeight, projection);

    // Update shaders
    setShaderProjectionMatrix(getShader(TEXT_SHADER_NAME), projection);
    setShaderProjectionMatrix(getShader(SPRITE_SHADER_NAME), projection);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    windowHeight = height;
    windowWidth = width;
    glViewport(0, 0, windowWidth, windowHeight);

    setProjectionMatrix();
}   

void errorCallback(int code, const char* error)
{
    printf("OpenGL Error %i: %s", code, error);
}