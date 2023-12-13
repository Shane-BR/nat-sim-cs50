#ifndef NS_SHADER
#define NS_SHADER

#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stdbool.h>

unsigned int newShader(const char* vertex_path, const char* frag_path, const char* name);

unsigned int getShader(const char* name);
void useShader(const unsigned int ID);

// utility uniform functions
void setShaderBool(const unsigned int shader_id, const char* name, const bool value);  
void setShaderInt(const unsigned int shader_id, const char* name, const int value);   
void setShaderFloat(const unsigned int shader_id, const char* name, const float value);
void setShaderMat4(const unsigned int shader_id, const char* name, const mat4 value);
void setShaderVec3(const unsigned int shader_id, const char* name, const vec3 value);
void setShaderVec4(const unsigned int shader_id, const char* name, const vec4 value);

#endif