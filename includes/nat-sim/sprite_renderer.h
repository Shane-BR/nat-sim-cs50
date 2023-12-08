#ifndef SPRITE_RENDERER
#define SPRITE_RENDERER

#include <cglm/types.h>

#define SPRITE_SHADER_FRAG_PATH "..\\resources\\shaders\\sprite_f.glsl"
#define SPRITE_SHADER_VERTEX_PATH "..\\resources\\shaders\\sprite_v.glsl"

#define SPRITE_SHADER_NAME "sprite"

void initSpriteRenderer(void);
void drawSprite(const char* texture, vec2 position, vec2 size, vec4 color, float rotate_degrees);
#endif