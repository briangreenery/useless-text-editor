// TextStyle.h

#ifndef TextStyle_h
#define TextStyle_h

#include "Integers.h"
#include <Windows.h>

class TextStyle
{
public:
	TextStyle();
	TextStyle( uint32   fontid,
	           COLORREF textColor,
	           COLORREF bkColor );

	uint32   fontid;
	COLORREF textColor;
	COLORREF bkColor;

	static const COLORREF useDefault = MAXDWORD;
};

#endif
