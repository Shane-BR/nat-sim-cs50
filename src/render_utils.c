#include "render_utils.h"
#include "shaders.h"
#include <math.h>

extern vec2 mapOriginPos;
extern vec2 tileSize;

void getNationColor(uint8_t nation, vec4* dest, float mix)
{
    switch (nation) 
    {
        case 0:
            setColor(COLOR_CYAN, dest);
            break;
                
        case 1:
            setColor(COLOR_BLUE, dest);
            break;
                
        case 2:
            setColor(COLOR_GREEN, dest);
            break;

        case 3:
            setColor(COLOR_RED, dest);
            break;

        default:
            setColor(COLOR_NONE, dest);
            break;
    }

    (*dest)[3] = mix;
}

void setColor(const vec4 color, vec4* dest)
{
    (*dest)[0] = color[0];
    (*dest)[1] = color[1];
    (*dest)[2] = color[2];
    (*dest)[3] = color[3];
}

float getTileStride()
{
    return tileSize[0] >= tileSize[1] ? tileSize[0]*2 : tileSize[1]*2;
}

void convertToScreenPosition(position pos, vec2* dest)
{
    float stride = getTileStride();
    (*dest)[0] = mapOriginPos[0] + (stride*pos.x);
    (*dest)[1] = mapOriginPos[1] + (stride*pos.y);
}

position getMapPositionFromScreenPos(vec2 screenPos)
{
    position pos;
    float stride = getTileStride();
    pos.x = round((screenPos[0] - mapOriginPos[0]) / stride);
    pos.y = round((screenPos[1] - mapOriginPos[1]) / stride);
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
void constructTexturedQuad(vec2 quadPos, vec2 quadDimensions, vec2 texturePortionPos, vec2 texturePortionDimensions, vec2 textureFullRes, float dest[QUAD_FLOAT_AMT])
{
    // V1 Top left
    dest[0] = quadPos[0];
    dest[1] = quadPos[1];
    dest[2] = texturePortionPos[0] / textureFullRes[0];
    dest[3] = texturePortionPos[1] / textureFullRes[1];

    // V2 Top right
    dest[4] = quadPos[0] + quadDimensions[0];
    dest[5] = quadPos[1];
    dest[6] = (texturePortionPos[0] + texturePortionDimensions[0]) / textureFullRes[0];
    dest[7] = texturePortionPos[1] / textureFullRes[1];

    // V3 Bottom left
    dest[8] = quadPos[0];
    dest[9] = quadPos[1] + quadDimensions[1];
    dest[10] = texturePortionPos[0] / textureFullRes[0];
    dest[11] = (texturePortionPos[1] + texturePortionDimensions[1]) / textureFullRes[1];

    // V4 Top right
    dest[12] = quadPos[0] + quadDimensions[0];
    dest[13] = quadPos[1];
    dest[14] = (texturePortionPos[0] + texturePortionDimensions[0]) / textureFullRes[0];
    dest[15] = texturePortionPos[1] / textureFullRes[1];

    // V5 Bottom left
    dest[16] = quadPos[0];
    dest[17] = quadPos[1] + quadDimensions[1];
    dest[18] = texturePortionPos[0] / textureFullRes[0];
    dest[19] = (texturePortionPos[1] + texturePortionDimensions[1]) / textureFullRes[1];

    // V6 Bottom right
    dest[20] = quadPos[0] + quadDimensions[0];
    dest[21] = quadPos[1] + quadDimensions[1];
    dest[22] = (texturePortionPos[0] + texturePortionDimensions[0]) / textureFullRes[0];
    dest[23] = (texturePortionPos[1] + texturePortionDimensions[1]) / textureFullRes[1];
} 

void setShaderProjectionMatrix(unsigned int shader, mat4 projection)
{
    useShader(shader);
    setShaderMat4(shader, "projection", projection);
}