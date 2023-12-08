#include <stdio.h>

#include <glad/glad.h>
#include "datatypes.h"
#include "shaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ARRAY_SIZE 512

static dict_node textures[ARRAY_SIZE];

unsigned int loadTexture(const char* path, const char* name)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
   
    if (!data)
    {
        printf("Failed to load texture at %s\n", path);
        printf("%s\n", stbi_failure_reason());
    }

    unsigned int id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    // Add to dict
    bool added = false;
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (textures[i].key == NULL)
        {
            textures[i].key = name;
            textures[i].value = id;
            added = true;
            break;
        }
    }

    if (!added)
    {
        printf("Unable to store texture, please provide larger buffer size.");
        exit(5);
    }

    return id;
}

unsigned int getTexture(const char* name)
{
    return textures[dictFind(textures, ARRAY_SIZE, name)].value;
}