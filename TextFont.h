// TextFont.h

#ifndef TextFont_h
#define TextFont_h

#include <Windows.h>
#include <usp10.h>
#include <string>
#include <memory>

class TextFont
{
public:
	TextFont( LPCWSTR name, HFONT hfont, HDC hdc );
	~TextFont();

	std::wstring          name;
	HFONT                 hfont;
	mutable SCRIPT_CACHE  fontCache;
	SCRIPT_FONTPROPERTIES fontProps;
};

typedef std::unique_ptr<TextFont> TextFontPtr;

#endif
