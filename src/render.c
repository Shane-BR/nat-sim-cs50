#include "render.h"
#include "constants.h"
#include "map.h"
#include "settlements.h"
#include "cursor.h"
#include "render_utils.h"
#include <math.h>
#include <stdint.h>


void renderMapTile(vec2 screenPos, vec4 color, tile* tile);
void renderSettlement(vec2 screenPos, vec4 color, settlement* stl);
void renderBorder(vec2 screenPos, vec4 color, tile* border_tile);
void renderUnit(vec2 screenPos, vec4 color, unit* unit);
void renderCursor();

vec2 mapOriginPos = {50.0f, 50.0f};

vec2 tileSize = {20.0f, 20.0f};

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

            renderMapTile(tilePos, (float*)COLOR_NONE, tile);

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

            tilePos[0] += getTileStride();
        }
        tilePos[1] += getTileStride();
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

    renderCursor();

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

    char texture[7] = "tile_0"; // Leave me alone
    texture[5] = (tile->traversability-1)+48;

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

void renderCursor()
{
    vec2 position;
    vec4 color;

    if (isCursorPressed())
        setColor(COLOR_GRAY, &color);
    else
        setColor(COLOR_NONE, &color);

    if (isCursorFocused())
    {
        convertToScreenPosition(getCursorFocusPoint(), &position);
        drawSprite("cursor_focus", position, tileSize, (float*)COLOR_NONE, 0);
    }

    convertToScreenPosition(getCursorPos(), &position);
    drawSprite("cursor", position, tileSize, color, 0);
}