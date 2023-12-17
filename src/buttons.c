#include "buttons.h"
#include "datatypes.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "render_utils.h"
#include "input.h"
#include <stdio.h>
#include <string.h>

#define MAX_UI_BUTTONS 32

typedef struct 
{
    char* title;
    char* texture_name;

    vec2 position;
    vec2 size;
    vec4 color;

    callback_func click_callback;
} button;

static button buttons[MAX_UI_BUTTONS];
static unsigned int buttons_size = 0;

// Called once every time you wanna add a button
void addButton(char* title, char* texture, const vec2 screen_pos, const vec2 size, const vec4 color, const callback_func click_callback)
{
    if (buttons_size >= MAX_UI_BUTTONS)
    {
        printf("Attempting to add too many UI buttons, MAX: %i\n", MAX_UI_BUTTONS);
        return;
    }

    button b;
    b.title = title;
    b.texture_name = texture;
    memcpy(&b.position, screen_pos, sizeof(vec2));
    memcpy(&b.size, size, sizeof(vec2));
    memcpy(&b.color, color, sizeof(vec4));
    b.click_callback = click_callback;

    buttons[buttons_size++] = b;
}

void renderButtons(void)
{
    for (int i = 0; i < buttons_size; i++)
    {
        button b = buttons[i];
        drawSprite(b.texture_name, b.position, b.size, b.color, 0);
        
        vec2 text_pos = {b.position[0] - b.size[0] + 5, b.position[1] - b.size[1]/2};
        renderText(b.title, text_pos);
    }
}

void checkButtonPressed(void)
{
    for (int i = 0; i < buttons_size; i++)
    {
        button b = buttons[i];

        vec2 upper_left = {b.position[0] - b.size[0], b.position[1] - b.size[1]};
        vec2 bottom_right = {b.position[0] + b.size[0], b.position[1] + b.size[1]};
        vec2 mouse_pos = {getMousePosition()[0], getMousePosition()[1]};

        // if in clickable bounds of button
        if (mouse_pos[0] <= bottom_right[0] && mouse_pos[1] <= bottom_right[1] &&
            mouse_pos[0] >= upper_left[0] && mouse_pos[1] >= upper_left[1])
        {
            b.click_callback();
        }
    }
}