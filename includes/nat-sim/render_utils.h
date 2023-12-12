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

void getNationColor(const uint8_t nation, vec4* dest, const float mix);
void setColor(const vec4 color, vec4* dest);
float getTileStride();
void convertToScreenPosition(const position pos, vec2* dest);
position getMapPositionFromScreenPos(const vec2 screenPos);
void constructTexturedQuad(const vec2 screenPos, const vec2 screenDimensions, const vec2 texturePos, const vec2 textureDimensions, const vec2 textureFullRes, const vec4 color, float dest[QUAD_FLOAT_AMT]);
void setShaderProjectionMatrix(const unsigned int shader, const mat4 projection);
void convertMat4(const float m4[4][4], float dest[16]);
#endif