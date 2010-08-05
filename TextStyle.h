// TextStyle.h

#ifndef TextStyle_h
#define TextStyle_h

#include "Integers.h"
#include <Windows.h>
#include <usp10.h>

class TextStyle
{
public:
	TextStyle();

	LONG lineHeight;
	LONG avgCharWidth;
	LONG tabSize;

	HFONT        font;
	SCRIPT_CACHE fontCache;

	HBRUSH gutterBrush;
	HBRUSH selectionBrush;
	HPEN   selectionHighlightPen;
};

#endif
