#ifndef RENDER
#define RENDER

#include "helpers.h"
#include "sprite_renderer.h"


void render(void);
position getMapPositionFromScreenPos(vec2 screenPos);
void convertToScreenPosition(position pos, vec2* dest);

#endif