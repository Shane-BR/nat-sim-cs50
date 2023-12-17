#include "render_utils.h"
#include "shaders.h"
#include <math.h>
#include <string.h>

extern const vec2 MAP_ORIGIN_POS;
extern const vec2 TILE_SIZE;

void getNationColor(const uint8_t nation, vec4* dest, const float mix)
{
    switch (nation) 
    {
        case 0:
            memcpy(dest, COLOR_CYAN, sizeof(vec4));
            break;
                
        case 1:
            memcpy(dest, COLOR_BLUE, sizeof(vec4));
            break;
                
        case 2:
            memcpy(dest, COLOR_GREEN, sizeof(vec4));
            break;

        case 3:
            memcpy(dest, COLOR_RED, sizeof(vec4));
            break;

        default:
            memcpy(dest, COLOR_NONE, sizeof(vec4));
            break;
    }

    if ((*dest)[3] > 0)
        (*dest)[3] = mix;
}

float getTileStride()
{
    return TILE_SIZE[0] >= TILE_SIZE[1] ? TILE_SIZE[0]*2 : TILE_SIZE[1]*2;
}

void convertToScreenPosition(const position pos, vec2* dest)
{
    float stride = getTileStride();
    (*dest)[0] = MAP_ORIGIN_POS[0] + (stride*pos.x);
    (*dest)[1] = MAP_ORIGIN_POS[1] + (stride*pos.y);
}

position getMapPositionFromScreenPos(const vec2 screenPos)
{
    position pos;
    float stride = getTileStride();
    pos.x = round((screenPos[0] - MAP_ORIGIN_POS[0]) / stride);
    pos.y = round((screenPos[1] - MAP_ORIGIN_POS[1]) / stride);
    return pos;
}

/**
 * @brief Constructs a textured quad.
 *
 * @param quadPos Position of the quad on the screen.
 * @param quadDimensions Dimensions of the quad.
 * @param texturePortionPos Start position in the texture for mapping.
 * @param texturePortionDimensions Dimensions of the texture portion to map.
 * @param textureFullRes Full resolution of the texture.
 * @param dest Array where the quad data will be stored.
 */
void constructTexturedQuad(const vec2 quadPos, const vec2 quadDimensions, const vec2 texturePortionPos, const vec2 texturePortionDimensions, const vec2 textureFullRes, const vec4 color, float dest[QUAD_FLOAT_AMT])
{
    
    // TODO Verrrry ugly fix later, for loop or some shit

    // V1 Top left
    dest[0] = quadPos[0];
    dest[1] = quadPos[1];
    dest[2] = texturePortionPos[0] / textureFullRes[0];
    dest[3] = texturePortionPos[1] / textureFullRes[1];
    dest[4] = color[0];
    dest[5] = color[1];
    dest[6] = color[2];
    dest[7] = color[3];

    // V2 Top right
    dest[8] = quadPos[0] + quadDimensions[0];
    dest[9] = quadPos[1];
    dest[10] = (texturePortionPos[0] + texturePortionDimensions[0]) / textureFullRes[0];
    dest[11] = texturePortionPos[1] / textureFullRes[1];
    dest[12] = color[0];
    dest[13] = color[1];
    dest[14] = color[2];
    dest[15] = color[3];

    // V3 Bottom left
    dest[16] = quadPos[0];
    dest[17] = quadPos[1] + quadDimensions[1];
    dest[18] = texturePortionPos[0] / textureFullRes[0];
    dest[19] = (texturePortionPos[1] + texturePortionDimensions[1]) / textureFullRes[1];
    dest[20] = color[0];
    dest[21] = color[1];
    dest[22] = color[2];
    dest[23] = color[3];

    // V4 Top right
    dest[24] = quadPos[0] + quadDimensions[0];
    dest[25] = quadPos[1];
    dest[26] = (texturePortionPos[0] + texturePortionDimensions[0]) / textureFullRes[0];
    dest[27] = texturePortionPos[1] / textureFullRes[1];
    dest[28] = color[0];
    dest[29] = color[1];
    dest[30] = color[2];
    dest[31] = color[3];

    // V5 Bottom left
    dest[32] = quadPos[0];
    dest[33] = quadPos[1] + quadDimensions[1];
    dest[34] = texturePortionPos[0] / textureFullRes[0];
    dest[35] = (texturePortionPos[1] + texturePortionDimensions[1]) / textureFullRes[1];
    dest[36] = color[0];
    dest[37] = color[1];
    dest[38] = color[2];
    dest[39] = color[3];

    // V6 Bottom right
    dest[40] = quadPos[0] + quadDimensions[0];
    dest[41] = quadPos[1] + quadDimensions[1];
    dest[42] = (texturePortionPos[0] + texturePortionDimensions[0]) / textureFullRes[0];
    dest[43] = (texturePortionPos[1] + texturePortionDimensions[1]) / textureFullRes[1];
    dest[44] = color[0];
    dest[45] = color[1];
    dest[46] = color[2];
    dest[47] = color[3];
} 

void setShaderProjectionMatrix(const unsigned int shader, const mat4 projection)
{
    useShader(shader);
    setShaderMat4(shader, "projection", projection);
}

void convertMat4(const float m4[4][4], float dest[16])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dest[(i*4)+j] = m4[i][j];
        }
    }
}