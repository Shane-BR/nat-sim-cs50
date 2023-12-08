#define _CRT_SECURE_NO_WARNINGS

#include "text_renderer.h"

#include <stdio.h>
#include <string.h>
#include <glad/glad.h>

#include "datatypes.h"
#include "textures.h"
#include "shaders.h"
#include "helpers.h"
#include "render_utils.h"

typedef struct
{
    int width;

    // Texture coords
    int texX;
    int texY;
    int texWidth;
    int texHeight;

    // Offset from origin
    int offsetX;
    int offsetY;
} characterData;

static const int CHARACTER_LIMIT = 1000;

static characterData characters[128];
static int lineHeight = 0;

static int textureRes[2] = {0, 0};

static unsigned int textShader, textVAO, textVBO;

void loadFontDescription(const char* path);
void initBuffers(void);

void loadFont()
{
    loadFontDescription(FONT_DESCRIPTION_PATH);
    loadTexture(FONT_TEXTURE_PATH, FONT_TEXTURE_NAME);
    initBuffers();
    textShader = newShader(TEXT_SHADER_VERTEX_PATH, TEXT_SHADER_FRAG_PATH, TEXT_SHADER_NAME);
}

void loadFontDescription(const char* path)
{
    FILE* fontDescription = fopen(path, "r");

    if (fontDescription == NULL)
    {
        printf("Error loading font");
        return;
    }

    char buffer[32] = {'\0'};
    char c;
    int curIndex = 0;
    int i = 0; 
    while ((c = fgetc(fontDescription)) != EOF)
    {
        if (c == '\n')
        {
            i = 0;
            continue;
        }
        else if (c == ' ')
        {
            buffer[i] = '\0';
            if (compareString(buffer, "page") || 
                compareString(buffer, "info"))
            {
                fscanf(fontDescription, "%*[^\n]"); // Skip line
            }
            i = 0;
            continue;
        }
        else if (c == '=')
        {
            int* param;
            buffer[i] = '\0';
            if (compareString(buffer, "id"))
                param = &curIndex;

            else if (compareString(buffer, "x"))
                param = &characters[(int)curIndex].texX;

            else if (compareString(buffer, "y"))
                param = &characters[(int)curIndex].texY;

            else if (compareString(buffer, "width"))
                param = &characters[(int)curIndex].texWidth;

            else if (compareString(buffer, "height"))
                param = &characters[(int)curIndex].texHeight;

            else if (compareString(buffer, "xadvance"))
                param = &characters[(int)curIndex].width;

            else if (compareString(buffer, "lineHeight"))
                param = &lineHeight;

            else if (compareString(buffer, "scaleW"))
                param = &textureRes[0];

            else if (compareString(buffer, "scaleH"))
                param = &textureRes[1];

            else if (compareString(buffer, "xoffset"))
                param = &characters[(int)curIndex].offsetX;

            else if (compareString(buffer, "yoffset"))
                param = &characters[(int)curIndex].offsetY;
            
            else 
            {
                i = 0;
                continue;
            }

            char valBuffer[16] = {'\0'};
            for (int j = 0; (c = fgetc(fontDescription)) != ' '; j++) 
            {
                if (c == EOF)
                    break;

                valBuffer[j] = c;
            }

            *param = strToInt(valBuffer);

            i = 0;
            continue;
        }

        buffer[i++] = c;

    }

    fclose(fontDescription);
}

void initBuffers(void)
{
    // Create buffers and setup vertex attribute pointers

    glGenVertexArrays(1, &textVAO);
    glBindVertexArray(textVAO);

    glGenBuffers(1, &textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*CHARACTER_LIMIT*QUAD_FLOAT_AMT, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);   
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderText(const char* text, vec2 screenPos, vec4 color, float scale)
{
    // Convert text to vertex data to push to the GPU

    int len = strlen(text);
    int vLen = len*QUAD_FLOAT_AMT;
    float charQuads[vLen];
    vec2 linePos = {0.0f, 0.0f};

    if (len > CHARACTER_LIMIT)
    {
        printf("Inputed text excedes the character limit of %i", CHARACTER_LIMIT);
        return;
    }

    for (int i = 0; i < len; i++)
    {
        char c = text[i];
        characterData charVals = characters[c];
        float xpos = (screenPos[0] + charVals.offsetX) + charVals.width * linePos[0];
        float ypos = (screenPos[1] + charVals.offsetY) + lineHeight * linePos[1];

        if (c == '\n') 
        {
            linePos[1]++;
            linePos[0] = 0;
            continue;
        }
        else if (c != ' ')
        {
            // Construct quad per letter
            float quad[QUAD_FLOAT_AMT];
            float quadScreenPos[] = {xpos, ypos};
            float quadScreenDimensions[] = {charVals.texWidth, charVals.texHeight};
            float texturePos[] = {charVals.texX, charVals.texY};
            float textureDimensions[] = {charVals.texWidth, charVals.texHeight};
            float textureResFloat[] = {(float)textureRes[0], (float)textureRes[1]};

            constructTexturedQuad(quadScreenPos, quadScreenDimensions, texturePos, textureDimensions, textureResFloat, quad);
        
            for (int j = 0; j < QUAD_FLOAT_AMT; j++)
            {
                charQuads[j + i*QUAD_FLOAT_AMT] = quad[j];
            }
        }

        linePos[0]++;
    }
    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vLen, charQuads);
    useShader(textShader);

    glBindTexture(GL_TEXTURE_2D, getTexture(FONT_TEXTURE_NAME));

    glBindVertexArray(textVAO);
    glDrawArrays(GL_TRIANGLES, 0, vLen);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}