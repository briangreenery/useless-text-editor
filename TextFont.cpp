// TextFont.cpp

#include "TextFont.h"

TextFont::TextFont( HFONT _font, HDC hdc )
	: font( _font )
	, fontCache( 0 )
{
	ZeroMemory( &fontProps, sizeof( fontProps ) );
	ScriptGetFontProperties( hdc, &fontCache, &fontProps );
}
