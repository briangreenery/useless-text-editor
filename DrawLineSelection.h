// DrawLineSelection.h

#ifndef DrawLineSelection_h
#define DrawLineSelection_h

#include "VisualLine.h"
#include <Windows.h>

void DrawLineSelection( HDC, RECT, const SelectionRanges& top, const SelectionRanges& bottom, bool isLast );

#endif
