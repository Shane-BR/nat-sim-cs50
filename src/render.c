#include "render.h"
#include "constants.h"
#include "helpers.h"
#include "sprite_renderer.h"
#include "map.h"
#include "settlements.h"
#include <stdint.h>


void renderMapTile(vec2 screenPos, vec4 color, tile* tile);
void renderSettlement(vec2 screenPos, vec4 color, settlement* stl);
void renderBorder(vec2 screenPos, vec4 color, tile* border_tile);
void renderUnit(vec2 screenPos, vec4 color, unit* unit);
void getNationColor(uint8_t nation, vec4* dest, float mix);
void setColor(vec4 color, vec4* dest);
void convertToScreenPosition(position pos, vec2* dest);

vec4 colorBlue  = {0.0f, 0.0f, 1.0f, 1.0f};
vec4 colorRed   = {1.0f, 0.0f, 0.0f, 1.0f};
vec4 colorGreen = {0.0f, 1.0f, 0.0f, 1.0f};
vec4 colorCyan  = {0.0f, 0.4f, 0.4f, 1.0f};
vec4 colorNone  = {0.0f, 0.0f, 0.0f, 0.0f};

const vec2 mapOriginPos = {50.0f, 50.0f};

vec2 tileSize = {20.0f, 20.0f};
float stride = 40.0f;

void render(void)
{
    vec2 tilePos = {mapOriginPos[0], mapOriginPos[1]}; 
    vec4 color;   
    for (int y = 0; y < MAP_SIZE; y++)
    { 
        for (int x = 0; x < MAP_SIZE; x++)
        {
            position pos = newPosition(x, y);
            tile* tile = getMapTile(pos);

            getNationColor(tile->ruling_nation, &color, 0.8f);

            renderMapTile(tilePos, colorNone, tile);

            // Is there a settlement or unit there
            settlement* stl = getSettlementFromPosition(pos);
            if (stl != NULL && stl->active)
            {
                renderSettlement(tilePos, color, stl);
            }

            if (tile->ruling_nation != -1)
            {
                renderBorder(tilePos, color, tile);
            }

            tilePos[0] += stride;
        }
        tilePos[1] += stride;
        tilePos[0] = mapOriginPos[0];
    }

    // Render units
    for (int n = 0; n < NAT_AMOUNT; n++)
    {
        nation nat = *getNation(n);
        getNationColor(n, &color, 0.4f);
        for (int u = 0; u < nat.units_amt; u++)
        {
            if (nat.units[u] == NULL)
                continue;

            unit* unit = nat.units[u];
            convertToScreenPosition(unit->position, &tilePos);
            renderUnit(tilePos, color, unit);
        }
    }

}

void renderMapTile(vec2 screenPos, vec4 color, tile* tile)
{
    float possible_rotations[4] =
    {
        0.0f,
        90.0f,
        180.0f,
        270.0f
    };

    char texture[7] = {'t', 'i', 'l', 'e', '_', (tile->traversability-1)+48}; // Leave me alone

    drawSprite(texture, screenPos, tileSize, color, 
            possible_rotations[(tile->position.x + tile->position.y) % 4]
        );

}

void renderSettlement(vec2 screenPos, vec4 color, settlement* stl)
{
    char* texture = stl->level == CITY ? "city" : "town";
    drawSprite(texture, screenPos, tileSize, color, 0);
}

void renderBorder(float* screenPos, vec4 color, tile* border_tile)
{

    // Neighbors and orderedRotations must be in the correct order
    position neighbors[4] =
    {        
        newPosition(0, -1),
        newPosition(1, 0),
        newPosition(0, 1),
        newPosition(-1, 0)
    };

    float orderedRotations[4] =
    {
        0.0f,
        90.0f,
        180.0f,
        270.0f
    };

    // Check each neighbor, and place borders in gaps
    for (int i = 0; i < 4; i++)
    {
        position npos;
        npos.x = border_tile->position.x + neighbors[i].x;
        npos.y = border_tile->position.y + neighbors[i].y;

        tile* nt = getMapTile(npos);

        if (nt == NULL || nt->ruling_nation != border_tile->ruling_nation)
        {
            drawSprite("border", screenPos, tileSize, color, orderedRotations[i]);
        }
    }
}

void renderUnit(vec2 screenPos, vec4 color, unit* unit)
{
    char* texture;

    switch (unit->unit_class) 
    {
        case SETTLER:
            texture = "settler";
            break;

        case MILITARY:
            texture = "army";
            break;

        default:
            break;
    }

    drawSprite(texture, screenPos, tileSize, color, 0);
}

void getNationColor(uint8_t nation, vec4* dest, float mix)
{
    switch (nation) 
    {
        case 0:
            setColor(colorCyan, dest);
            break;
                
        case 1:
            setColor(colorBlue, dest);
            break;
                
        case 2:
            setColor(colorGreen, dest);
            break;

        case 3:
            setColor(colorRed, dest);
            break;

        default:
            setColor(colorNone, dest);
            break;
    }

    (*dest)[3] = mix;
}

void setColor(vec4 color, vec4* dest)
{
    (*dest)[0] = color[0];
    (*dest)[1] = color[1];
    (*dest)[2] = color[2];
    (*dest)[3] = color[3];
}

void convertToScreenPosition(position pos, vec2* dest)
{
    (*dest)[0] = mapOriginPos[0] + (stride*pos.x);
    (*dest)[1] = mapOriginPos[1] + (stride*pos.y);
}