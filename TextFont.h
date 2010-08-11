// TextFont.h

#ifndef TextFont_h
#define TextFont_h

#include <Windows.h>
#include <usp10.h>

class TextFont
{
public:
	TextFont( HFONT font, HDC hdc );

	HFONT                 font;
	SCRIPT_CACHE          fontCache;
	SCRIPT_FONTPROPERTIES fontProps;
};

#endif
