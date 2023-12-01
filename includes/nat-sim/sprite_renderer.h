#ifndef SPRITE_RENDERER
#define SPRITE_RENDERER

#include <cglm/types.h>

void initSpriteRenderer(void);
void drawSprite(const char* texture, vec2 position, vec2 size, vec4 color, float rotate_degrees);
#endif