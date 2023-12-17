#ifndef NS_BUTTONS
#define NS_BUTTONS

#include <cglm/types.h>

typedef void (*callback_func)(void);

void addButton(char* title, char* texture, const vec2 screen_pos, const vec2 size, const vec4 color, const callback_func click_callback);
void renderButtons(void);
void checkButtonPressed(void);

#endif