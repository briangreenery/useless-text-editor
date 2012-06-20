// TextStyle.h

#ifndef TextStyle_h
#define TextStyle_h

#include <stdint.h>
#include <Windows.h>

class TextStyle
{
public:
	TextStyle();
	TextStyle( uint32_t   fontid,
	           COLORREF textColor,
	           COLORREF bkColor );

	uint32_t fontid;
	COLORREF textColor;
	COLORREF bkColor;

	static const COLORREF useDefault = MAXDWORD;
};

#endif
