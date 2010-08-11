// TextFont.h

#ifndef TextFont_h
#define TextFont_h

#include <Windows.h>
#include <usp10.h>
#include <string>

class TextFont
{
public:
	TextFont( LPCWSTR name, HFONT font, HDC hdc );

	std::wstring          name;
	HFONT                 font;
	SCRIPT_CACHE          fontCache;
	SCRIPT_FONTPROPERTIES fontProps;
};

#endif
