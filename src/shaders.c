#include "shaders.h"
#include "datatypes.h"
#include "render_utils.h"

#include <glad/glad.h>

#include <stdlib.h>
#include <stdio.h>

#define ARRAY_SIZE 16

static dict_node shaders[ARRAY_SIZE];

unsigned int newShader(const char* vertex_path, const char* frag_path, const char* name)
{
    unsigned int id;

    FILE* vertex_file = fopen(vertex_path, "r");
    FILE* frag_file = fopen(frag_path, "r");

    if (vertex_file == NULL)
        return 0;

    if (frag_file == NULL)
        return 0;

    

    // Write strings
    const int BUF_SIZE = 1024;
    char vertex_code[BUF_SIZE], frag_code[BUF_SIZE];
    int length = 0;
    char c;
    for (int i = 0; (c = fgetc(vertex_file)) != EOF; i++)
    {
        vertex_code[i] = c;
        length++;
    }
    vertex_code[length] = '\0';

    length = 0;
    for (int i = 0; (c = fgetc(frag_file)) != EOF; i++) 
    {
        frag_code[i] = c;
        length++;
    }
    frag_code[length] = '\0';

    fclose(vertex_file);
    fclose(frag_file);

    const char* VCODE = vertex_code;
    const char* FCODE = frag_code;

    unsigned int vertex, fragment;
    int success;
    char info_log[BUF_SIZE];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &VCODE, NULL);
    glCompileShader(vertex);

    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, BUF_SIZE, NULL, info_log);
        printf("Vertex Shader: %s\n\n", info_log);
    }

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &FCODE, NULL);
    glCompileShader(fragment);

    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, BUF_SIZE, NULL, info_log);
        printf("Fragment Shader: %s\n\n", info_log);
    }

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id); 

    // print compile errors if any
    glGetShaderiv(id, GL_LINK_STATUS, &success); 
    if(!success)
    {
        glGetShaderInfoLog(id, BUF_SIZE, NULL, info_log);
        printf("Linking failed: %s\n\n", info_log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Add to dict
    bool added = false;
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (shaders[i].key == NULL)
        {
            shaders[i].key = name;
            shaders[i].value = id;
            added = true;
            break;
        }
    }

    if (!added)
    {
        printf("Unable to store shader, please provide larger buffer size.");
        exit(5);
    }

    return id;
}

unsigned int getShader(const char* name)
{
    return shaders[dictFind(shaders, ARRAY_SIZE, name)].value;
}

void useShader(const unsigned int ID)
{
    glUseProgram(ID);
}

void setShaderBool(const unsigned int shader_id, const char* name, const bool value)
{         
    glUniform1i(glGetUniformLocation(shader_id, name), (int)value); 
}

void setShaderInt(const unsigned int shader_id, const char* name, const int value)
{ 
    glUniform1i(glGetUniformLocation(shader_id, name), value); 
}

void setShaderFloat(const unsigned int shader_id, const char* name, const float value)
{ 
    glUniform1f(glGetUniformLocation(shader_id, name), value); 
} 

void setShaderMat4(const unsigned int shader_id, const char* name, const mat4 value)
{ 
    float val_converted[16]; convertMat4(value, val_converted);
    unsigned int loc = glGetUniformLocation(shader_id, name);

    glUniformMatrix4fv(loc, 1, GL_FALSE, val_converted); 
} 

void setShaderVec3(const unsigned int shader_id, const char* name, const vec3 value)
{
    glUniform3fv(glGetUniformLocation(shader_id, name), 1, value); 
}

void setShaderVec4(const unsigned int shader_id, const char* name, const vec4 value)
{
    glUniform4fv(glGetUniformLocation(shader_id, name), 1, value); 
}