#include "window.h"
#include "cglm/cam.h"
#include "sprite_renderer.h"
#include "helpers.h"
#include "shaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#define GLEW_STATIC

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
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetErrorCallback(errorCallback);
    initSpriteRenderer();

    glm_mat4_identity(projection);
    setProjectionMatrix();

}

int shouldWindowClose(void)
{
    return glfwWindowShouldClose(window);
}

void updateWindow(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    vec2 pos = {0.0f, 0.0f};
    vec2 size = {1.0f, 1.0f};
    vec3 color = {0, 0, 0};
    
    drawSprite("tile_0", pos, size, color);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void terminateWindow(void)
{
    glfwTerminate();
}

void setProjectionMatrix(void)
{
    //glm_ortho(0.0f, 100.0f, 100.0f, 0.0f, -1.0f, 1.0f, projection);
    glm_ortho_default((float)windowWidth / windowHeight, projection);

    // Update shaders
    int shader_id = getShader("sprite");
    useShader(shader_id);
    setShaderMat4(shader_id, "projection", projection);
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