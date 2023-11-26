#include "shaders.h"
#include "textures.h"

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>

unsigned int spriteVAO;
unsigned int shader;

void loadSpriteTextures();

void initSpriteRenderer(void)
{
    unsigned int VBO, EBO;
    float vertices[] = 
    {
        // Position                         Tex Coords
        
        // positions
        -0.7f, 0.7f, 0.0f,     0.0f, 1.0f,    // Top left
        -0.7, -0.7, 0.0f,      0.0f, 0.0f,    // Bottom left
        0.7, 0.7, 0.0f,    1.0f, 1.0f,   // Top right
        0.7, -0.7, 0.0f,   1.0f, 0.0f    // Bottom right
    };
 
    unsigned int indices[] = 
    {
        0, 1, 3,   // first triangle
        0, 2, 3    // second triangle
    };

    // Init shader
    shader = newShader("shaders/sprite_v.glsl", "shaders/sprite_f.glsl", "sprite");

    glGenVertexArrays(1, &spriteVAO);
    glBindVertexArray(spriteVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);   

    loadSpriteTextures();

}

// Place all potential textures in here
void loadSpriteTextures()
{
    loadTexture("..\\resources\\textures\\tile-0.png", "tile_0");
}

void drawSprite(const char* texture, vec2 position, vec2 size, vec3 color)
{
    mat4 model; glm_mat4_identity(model);
    glm_translate(model, position); 
    glm_scale(model, size);
    useShader(shader);

    setShaderMat4(shader, "model", model);
    //setShaderVec3(shader, "color", color);
    

    glBindTexture(GL_TEXTURE_2D, getTexture(texture));

    glBindVertexArray(spriteVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}