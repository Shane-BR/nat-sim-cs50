#ifndef SPRITE_RENDERER
#define SPRITE_RENDERER

#include <cglm/types.h>

#define SPRITE_SHADER_FRAG_PATH "..\\resources\\shaders\\sprite_f.glsl"
#define SPRITE_SHADER_VERTEX_PATH "..\\resources\\shaders\\sprite_v.glsl"

#define SPRITE_SHADER_NAME "sprite"

void initSpriteRenderer(void);
void drawSprite(const char* texture, const vec2 position, const vec2 size, const vec4 color, const float rotate_degrees);
#endif