#ifndef NS_TEXT_RENDERER
#define NS_TEXT_RENDERER

#include <cglm/types.h>

#define FONT_DESCRIPTION_PATH "..\\resources\\fonts\\dos437.fnt"
#define FONT_TEXTURE_PATH "..\\resources\\fonts\\dos437.png"
#define TEXT_SHADER_FRAG_PATH "..\\resources\\shaders\\text_f.glsl"
#define TEXT_SHADER_VERTEX_PATH "..\\resources\\shaders\\text_v.glsl"

#define TEXT_SHADER_NAME "text"
#define FONT_TEXTURE_NAME "font"

void loadFont();
void renderText(const char* text, const vec2 position);

#endif