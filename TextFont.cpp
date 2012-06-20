// TextFont.cpp

#include "TextFont.h"

class CouldNotCreateFont {};

TextFont::TextFont( const std::wstring& name, int size, bool bold, bool italic )
	: name( name )
	, size( size )
	, bold( bold )
	, italic( italic )
	, hfont( 0 )
	, fontCache( 0 )
{
	HDC hdc = GetDC( NULL );

	LOGFONT logFont = {};
	logFont.lfHeight = -MulDiv( size, GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
	wcscpy_s( logFont.lfFaceName, name.c_str() );

	hfont = CreateFontIndirect( &logFont );
	if ( hfont == 0 )
	{
		ReleaseDC( NULL, hdc );
		throw CouldNotCreateFont();
	}

	SelectObject( hdc, hfont );
	ZeroMemory( &fontProps, sizeof( fontProps ) );
	ScriptGetFontProperties( hdc, &fontCache, &fontProps );

	ReleaseDC( NULL, hdc );
}

TextFont::~TextFont()
{
	ScriptFreeCache( &fontCache );
	DeleteObject( hfont );
}
