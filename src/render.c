#define _CRT_NONSTDC_NO_WARNINGS
#include "render.h"
#include "constants.h"
#include "map.h"
#include "settlements.h"
#include "cursor.h"
#include "render_utils.h"
#include "helpers.h"
#include "sim_log.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "sim_time.h"
#include "borders.h"
#include "sim_time.h"
#include "buttons.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>


void renderMapTile(vec2 screenPos, vec4 color, tile* tile);
void renderSettlement(vec2 screenPos, vec4 color, settlement* stl);
void renderBorder(vec2 screenPos, vec4 color, tile* border_tile);
void renderUnit(vec2 screenPos, vec4 color, unit* unit);
void renderCursor(void);
void renderDateAndTime(void);
void renderFocusedTileText(void);
void renderLogBox(void);
void renderBorderWorkerCount(const border b);
void renderFPS(void);
void renderTotalPopulation(void);

const vec2 MAP_ORIGIN_POS = {50.0f, 50.0f};

const vec2 TILE_SIZE = {20.0f, 20.0f};

const static vec2 TILE_INFO_POS = {700.0f, 90.0f};
const static vec2 STL_INFO_POS = {700.0f, 350.0f};
const static vec2 DATE_TIME_POS = {5.0f, 5.0f};
const static vec2 TOTAL_POP_POS = {500.0f, 5.0f};

const static vec2 LOG_BOX_SIZE = {280.0f, 72.0f};
const static vec2 LOG_BOX_POS = {955.0f, 600.0f};
const static vec2 LOG_MSG_POS = {700.0f, 540.0f};
const static unsigned int MAX_MSG_DISPLAYED = 8;

static const float TEXT_UPDATE_DELAY = 0.5f;

static const vec2 SPEED_BUTTON_POS = {1195.0f, 50.0f};

static const vec2 FPS_POS = {1195.0f, 125.0f};

void initRenderer()
{
    // Add speed adjust button
    addButton("SPD", "button", SPEED_BUTTON_POS, TILE_SIZE, COLOR_NONE, updateTPS);
}

void render(void)
{
    vec2 tilePos = {MAP_ORIGIN_POS[0], MAP_ORIGIN_POS[1]}; 
    vec4 color;   
    // TODO redo this with batch renderering and the like
    // Possibly rather than getting everything EVERY frame and rendering that,
    // use a system in which each "new thing" call updates a static array, "render queue".
    // It would save a lot of CPU cycles.
    for (int y = 0; y < MAP_SIZE; y++)
    { 
        for (int x = 0; x < MAP_SIZE; x++)
        {
            position pos = {x, y};
            tile* tile = getMapTile(pos);

            getNationColor(tile->ruling_nation, &color, 0.8f);

            renderMapTile(tilePos, (float*)COLOR_NONE, tile);

            if (tile->ruling_nation != -1)
            {
                renderBorder(tilePos, color, tile);

                border* b;
                if ((b = getBorderFromPosition(pos)) != NULL)
                {
                    renderBorderWorkerCount(*b);
                }
            }

            // Is there a settlement or unit there
            settlement* stl = getSettlementFromPosition(pos);
            if (stl != NULL && stl->active)
            {
                renderSettlement(tilePos, color, stl);
            }

            tilePos[0] += getTileStride();
        }
        tilePos[1] += getTileStride();
        tilePos[0] = MAP_ORIGIN_POS[0];
    }

    // Render units
    for (int n = 0; n < NAT_AMOUNT; n++)
    {
        nation nat = *getNation(n);
        getNationColor(n, &color, 0.4f);
        for (int u = 0; u < nat.units_amt; u++)
        {
            unit* unit = nat.units[u];
            if (unit == NULL)
                continue;

            convertToScreenPosition(unit->position, &tilePos);
            renderUnit(tilePos, color, unit);
        }
    }

    renderCursor();

    // Text
    renderDateAndTime();

    if (isCursorFocused())
        renderFocusedTileText();

    renderLogBox();

    renderButtons();

    //renderFPS();

    renderTotalPopulation();

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

    drawSprite(texture, screenPos, TILE_SIZE, color, 
            possible_rotations[(tile->position.x + tile->position.y) % 4]
        );

}

void renderSettlement(vec2 screenPos, vec4 color, settlement* stl)
{
    char* texture = stl->level == CITY ? "city" : "town";
    drawSprite(texture, screenPos, TILE_SIZE, color, 0);
}

void renderBorder(float* screenPos, vec4 color, tile* border_tile)
{

    // Neighbors and orderedRotations must be in the correct order
    position neighbors[4] =
    {        
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0}
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
            drawSprite("border", screenPos, TILE_SIZE, color, orderedRotations[i]);
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

    drawSprite(texture, screenPos, TILE_SIZE, color, 0);
}

void renderCursor()
{
    vec2 position;

    if (isCursorFocused())
    {
        convertToScreenPosition(getCursorFocusPoint(), &position);
        drawSprite("cursor_focus", position, TILE_SIZE, COLOR_NONE, 0);
    }

    convertToScreenPosition(getCursorPos(), &position);
    drawSprite("cursor", position, TILE_SIZE, COLOR_NONE, 0);
}

void renderDateAndTime()
{
    unsigned int ticks = getTicks();
    int day = 1 + ((ticks / TICKS_PER_DAY) % 365);
    int year = (ticks / TICKS_PER_DAY) / 365;

    char dateTime[64] = {'\0'};
    sprintf(dateTime, "Nat-Sim          Day: %i     Year: %i", day, year);
    renderText(dateTime, DATE_TIME_POS);
}

void renderFocusedTileText()
{
    position pos = getCursorFocusPoint();
    tile focus = *getMapTile(pos);
    
    char tileInfo[256] = {'\0'};

    char* natName = getNationName(focus.ruling_nation);

    vec4 natColor; 
    getNationColor(focus.ruling_nation, &natColor, 0.8f);
    sprintf(tileInfo, "    Tile Info\n"
                      "Traverseability: %i/%i\n"
                      "Survivability: %i/%i\n\n"
                      "Food Abundance: %i/%i\n"
                      "Material Abundance: %i/%i\n"
                      "Ruling Nation: ^%i,%i,%i,%i;%s^;\n"
                      "X: %i\n"
                      "Y: %i", 
                      focus.traversability, MAX_TRAVERSABILITY,
                      focus.survivability, UINT8_MAX,
                      focus.food_abundance, UINT8_MAX,
                      focus.material_abundance, UINT8_MAX,
                      (int)(natColor[0]*UINT8_MAX), 
                            (int)(natColor[1]*UINT8_MAX), 
                            (int)(natColor[2]*UINT8_MAX), 
                            (int)(natColor[3]*UINT8_MAX), 
                      natName == NULL ? "None" : natName,
                      pos.x,
                      pos.y);

    renderText(tileInfo, TILE_INFO_POS); 

    settlement* stl = getSettlementFromPosition(pos);
    static float delay = 0.0f;
    static char stlInfo[256] = {'\0'};

    // Update cur_food and cur_mat every DELAY_R
    if (stl == NULL)
        return;

    if (!stl->active)
        return;

    static settlement* cur_stl;
    if (((delay -= getDeltaTime()) <= 0.0f || stl != cur_stl))
    {
        cur_stl = stl;
        sprintf(stlInfo, "    Settlement Statistics\n"
                                "Settlement Level: %i\n"
                                "Population: %i\n"
                                "Food: %i\n"
                                "Materials: %i\n"
                                "Infrastructure: %i/%i\n"
                                "Morale: %i/%i\n"
                                "Cultivation Efficiency: %i/%i",
                                cur_stl->level,
                                cur_stl->local_population,
                                (int)cur_stl->food,
                                (int)cur_stl->materials,
                                (int)cur_stl->local_infrastructure, UINT8_MAX,
                                cur_stl->local_morale, UINT8_MAX,
                                cur_stl->cultivation_efficiency, UINT8_MAX);

        delay = TEXT_UPDATE_DELAY;
    }
    renderText(stlInfo, STL_INFO_POS);
}

void renderLogBox()
{
    // Render the box
    drawSprite("text_box", LOG_BOX_POS, LOG_BOX_SIZE, (float*)COLOR_NONE, 0);

    // Loop through all logs and render them with gaps
    int yOffset = 15;
    vec2 curLogMsgPos = {LOG_MSG_POS[0], LOG_MSG_POS[1]};
    for (int i = 0; i < MAX_MSG_DISPLAYED; i++)
    {
        char* log = getLog(i);
        
        if (log == NULL)
            continue;

        renderText(log, curLogMsgPos);
        curLogMsgPos[1] += yOffset;
    }

}

void renderBorderWorkerCount(const border b)
{
    char num[3] = {'\0'};
    const vec2 offset = {-3.0f, -8.0f};
    vec2 pos; 

    sprintf(num, "%i", b.workers_count);

    convertToScreenPosition(b.tile->position, &pos);
    pos[0] += offset[0];
    pos[1] += offset[1];

    renderText(num, pos);

}

void renderFPS(void)
{
    static float delay = 0;
    static char fps[16];
    if ((delay -= getDeltaTime()) <= 0.0f)
    {
        sprintf(fps, "%i", (int)getFPS());
        delay = TEXT_UPDATE_DELAY;
    }

    renderText(fps, FPS_POS);

}

void renderTotalPopulation(void)
{
    static char pop[64];
    static int delay = 0;

    if ((delay -= getDeltaTime()) <= 0.0f)
    {
        // Just humans for now
        sprintf(pop, "Total Population: %i", getNation(1)->population);
        delay = TEXT_UPDATE_DELAY;
    }
    
    renderText(pop, TOTAL_POP_POS);
}