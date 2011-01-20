// TextFont.cpp

#include "TextFont.h"

TextFont::TextFont( LPCWSTR _name, HFONT _hfont, HDC hdc )
	: name( _name )
	, hfont( _hfont )
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
