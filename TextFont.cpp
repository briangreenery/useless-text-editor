// TextFont.cpp

#include "TextFont.h"

TextFont::TextFont( LPCWSTR _name, HFONT _font, HDC hdc )
	: name( _name )
	, font( _font )
	, fontCache( 0 )
{
	ZeroMemory( &fontProps, sizeof( fontProps ) );
	ScriptGetFontProperties( hdc, &fontCache, &fontProps );
}
