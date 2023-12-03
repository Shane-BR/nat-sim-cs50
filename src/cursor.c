#include "map.h"
#include "cursor.h"

position cursorPos = {0, 0};
position focusPoint = {0, 0};
bool focusedOnPos = false;
bool cursorPressed = false;

position getCursorPos(void)
{
    return cursorPos;
}

void setCursorPosition(position pos)
{
    if (inMapBounds(pos))
        cursorPos = pos;
}

void incrementCursorX(void)
{
    position newPos = cursorPos; newPos.x++;

    if (inMapBounds(newPos))
        cursorPos = newPos;
}

void incrementCursorY(void)
{
    position newPos = cursorPos; newPos.y++;

    if (inMapBounds(newPos))
        cursorPos = newPos;
}

void decrementCursorX(void)
{
    position newPos = cursorPos; newPos.x--;

    if (inMapBounds(newPos))
        cursorPos = newPos;
}

void decrementCursorY(void)
{
    position newPos = cursorPos; newPos.y--;

    if (inMapBounds(newPos))
        cursorPos = newPos;
}

position getFocusPoint(void)
{
    return focusPoint;
}

void setCursorFocusPoint(position pos)
{
    if (inMapBounds(pos))
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