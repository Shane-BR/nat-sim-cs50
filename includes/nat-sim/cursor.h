#ifndef NS_CURSOR
#define NS_CURSOR

#include "datatypes.h"

position getCursorPos(void);
void setCursorPosition(position pos);
void incrementCursorX(void);
void incrementCursorY(void);
void decrementCursorX(void);
void decrementCursorY(void);
void setCursorFocusPoint(void);
bool isCursorFocused(void);
void setCursorPressedState(bool pressed);
bool isCursorVisable(void);
void setCursorVisable(bool visable);
position getCursorFocusPoint(void);
#endif