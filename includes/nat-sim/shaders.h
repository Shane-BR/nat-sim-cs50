#ifndef SHADER
#define SHADER

#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stdbool.h>

unsigned int newShader(const char* vertex_path, const char* frag_path, const char* name);

unsigned int getShader(const char* name);
void useShader(unsigned int ID);

// utility uniform functions
void setShaderBool(unsigned int shader_id, const char* name, bool value);  
void setShaderInt(unsigned int shader_id, const char* name, int value);   
void setShaderFloat(unsigned int shader_id, const char* name, float value);
void setShaderMat4(unsigned int shader_id, const char* name, mat4 value);
void setShaderVec3(unsigned int shader_id, const char* name, vec3 value);
void setShaderVec4(unsigned int shader_id, const char* name, vec4 value);

#endif