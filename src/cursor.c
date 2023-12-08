#include "map.h"
#include "cursor.h"

static position cursorPos = {0, 0};
static position focusPoint = {0, 0};
static bool focusedOnPos = false;
static bool cursorPressed = false;
static bool mouseInMapBounds = true;
static bool cursorVisable = true;

position getCursorPos(void)
{
    return cursorPos;
}

void setCursorPosition(position pos)
{
    mouseInMapBounds = inMapBounds(pos);
    if (mouseInMapBounds)
        cursorPos = pos;
}

void incrementCursorX(void)
{
    position newPos = cursorPos; newPos.x++;
    setCursorPosition(newPos);
}

void incrementCursorY(void)
{
    position newPos = cursorPos; newPos.y++;
    setCursorPosition(newPos);
}

void decrementCursorX(void)
{
    position newPos = cursorPos; newPos.x--;
    setCursorPosition(newPos);
}

void decrementCursorY(void)
{
    position newPos = cursorPos; newPos.y--;
    setCursorPosition(newPos);
}

position getFocusPoint(void)
{
    return focusPoint;
}

void setCursorFocusPoint(position pos)
{
    if (mouseInMapBounds)
    {
        focusPoint = pos;
        focusedOnPos = true;
    }
    else 
    {
        focusedOnPos = false;
    }
}

bool isCursorFocused(void)
{
    return focusedOnPos;
}

bool isCursorPressed(void)
{
    return cursorPressed;
}

position getCursorFocusPoint(void)
{
    return focusPoint;
}

void setCursorPressedState(bool pressed)
{
    cursorPressed = pressed;
    setCursorFocusPoint(cursorPos);
}