// TextFont.cpp

#include "TextFont.h"

TextFont::TextFont( LPCWSTR name, HFONT hfont, HDC hdc )
	: name( name )
	, hfont( hfont )
	, fontCache( 0 )
{
	ZeroMemory( &fontProps, sizeof( fontProps ) );
	ScriptGetFontProperties( hdc, &fontCache, &fontProps );
}

TextFont::~TextFont()
{
	ScriptFreeCache( &fontCache );
	DeleteObject( hfont );
}
