// TextStyle.h

#ifndef TextStyle_h
#define TextStyle_h

#include "Integers.h"
#include "UString.h"
#include "ArrayOf.h"
#include <Windows.h>
#include <MLang.h>
#include <usp10.h>
#include <vector>

class TextFont
{
public:
	TextFont( HFONT _font, DWORD _codePages )
		: font( _font )
		, fontCache( 0 )
		, codePages( _codePages )
	{}

	HFONT        font;
	SCRIPT_CACHE fontCache;
	DWORD        codePages;
};

class TextStyle
{
public:
	TextStyle();

	int FontFallback( HDC, int style, UTF16Ref );
	bool HasMissingGlyphs( int style, ArrayOf<WORD> glyphs ) const;

	int lineHeight;
	int avgCharWidth;
	int tabSize;

	HBRUSH gutterBrush;
	HBRUSH selectionBrush;

	std::vector<TextFont> fonts;

private:
	IMLangFontLink2* fontLink;
};

#endif
