#ifndef RENDER_UTILS
#define RENDER_UTILS

#include <cglm/types.h>
#include "datatypes.h"

static const vec4 COLOR_BLUE   = {0.0f, 0.0f, 1.0f, 1.0f};
static const vec4 COLOR_RED    = {1.0f, 0.0f, 0.0f, 1.0f};
static const vec4 COLOR_GREEN  = {0.0f, 1.0f, 0.0f, 1.0f};
static const vec4 COLOR_CYAN   =  {0.0f, 0.4f, 0.4f, 1.0f};
static const vec4 COLOR_GRAY   =  {0.5f, 0.5f, 0.5f, 1.0f};
static const vec4 COLOR_NONE   =  {0.0f, 0.0f, 0.0f, 0.0f};

static const int QUAD_FLOAT_AMT = 48;

void getNationColor(uint8_t nation, vec4* dest, float mix);
void setColor(const vec4 color, vec4* dest);
float getTileStride();
void convertToScreenPosition(position pos, vec2* dest);
position getMapPositionFromScreenPos(vec2 screenPos);
void constructTexturedQuad(vec2 screenPos, vec2 screenDimensions, vec2 texturePos, vec2 textureDimensions, vec2 textureFullRes, vec4 color, float dest[QUAD_FLOAT_AMT]);
void setShaderProjectionMatrix(unsigned int shader, mat4 projection);
void convertMat4(float m4[4][4], float dest[16]);
#endif