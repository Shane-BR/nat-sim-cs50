#include "sprite_renderer.h"
#include "shaders.h"
#include "textures.h"

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>

static unsigned int spriteVAO;
static unsigned int spriteShader;

void loadSpriteTextures();

void initSpriteRenderer(void)
{
    unsigned int VBO, EBO;
    float vertices[] = 
    {
        // Position                         Tex Coords
        
        // positions
        -1.0f, 1.0f, 0.0f,     0.0f, 1.0f,    // Top left
        -1.0f, -1.0f, 0.0f,      0.0f, 0.0f,    // Bottom left
        1.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // Top right
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f    // Bottom right
    };
 
    unsigned int indices[] = 
    {
        0, 1, 2,   // first triangle
        1, 2, 3    // second triangle
    };

    // Init shader
    spriteShader = newShader(SPRITE_SHADER_VERTEX_PATH, SPRITE_SHADER_FRAG_PATH, SPRITE_SHADER_NAME);

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

    glBindVertexArray(0);   
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    loadSpriteTextures();

}

// Place all potential textures in here
void loadSpriteTextures()
{
    loadTexture("..\\resources\\textures\\tile-0.png", "tile_0");
    loadTexture("..\\resources\\textures\\tile-1.png", "tile_1");
    loadTexture("..\\resources\\textures\\tile-2.png", "tile_2");
    loadTexture("..\\resources\\textures\\tile-3.png", "tile_3");
    loadTexture("..\\resources\\textures\\town.png", "town");
    loadTexture("..\\resources\\textures\\city.png", "city");
    loadTexture("..\\resources\\textures\\border_line.png", "border");
    loadTexture("..\\resources\\textures\\settler_horse.png", "settler");
    loadTexture("..\\resources\\textures\\cursor.png", "cursor");
    loadTexture("..\\resources\\textures\\cursor_focus.png", "cursor_focus");
    loadTexture("..\\resources\\textures\\text_box.png", "text_box");
}

void drawSprite(const char* texture, vec2 position, vec2 size, vec4 color, float rotate_degrees)
{
    mat4 model;             glm_mat4_identity(model);
    vec3 position_vec3  =   {position[0], position[1], 0.0f};
    vec3 size_vec3      =   {size[0], size[1], 0.0f};
    vec3 rotation_axis  =   {0.0f, 0.0f, 1.0f};

    glm_translate(model, position_vec3); 
    glm_rotate(model, glm_rad(rotate_degrees), rotation_axis);
    glm_scale(model, size_vec3);
    useShader(spriteShader);

    setShaderMat4(spriteShader, "model", model);
    setShaderVec4(spriteShader, "color", color);

    glBindTexture(GL_TEXTURE_2D, getTexture(texture));

    glBindVertexArray(spriteVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}