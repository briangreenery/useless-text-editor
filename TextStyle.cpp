// TextStyle.cpp

#include "TextStyle.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

#undef min
#undef max

TextStyle::TextStyle()
	: fontSize( 28 )
	, defaultFont( 0 )
	, lineHeight( 0 )
	, avgCharWidth( 0 )
	, tabSize( 0 )
{
	AddFont( L"Consolas" );
	if ( fonts.empty() )
		AddFont( L"Courier New" );

	AddFallbackFonts();

	gutterBrush    = CreateSolidBrush( RGB( 236, 236, 236 ) );
	selectionBrush = CreateSolidBrush( RGB( 173, 214, 255 ) );

	SetDefaultFont( 0 );
}

TextStyle::~TextStyle()
{
	for ( auto it = fonts.begin(); it != fonts.end(); ++it )
	{
		ScriptFreeCache( &it->fontCache );
		DeleteObject( it->font );
	}

	DeleteObject( gutterBrush );
	DeleteObject( selectionBrush );
}

void TextStyle::SetDefaultFont( size_t font )
{
	defaultFont = font;

	HDC hdc = GetDC( NULL );

	SelectObject( hdc, fonts[font].font );

	TEXTMETRIC tm;
	GetTextMetricsW( hdc, &tm );

	ReleaseDC( NULL, hdc );

	avgCharWidth = tm.tmAveCharWidth;
	tabSize = 4 * tm.tmAveCharWidth;
}

size_t TextStyle::AddFont( LPCWSTR name )
{
	HDC hdc = GetDC( NULL );

	LOGFONT logFont = {};
	logFont.lfHeight = -MulDiv( fontSize, GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
	wcscpy_s( logFont.lfFaceName, name );

	HFONT font = CreateFontIndirect( &logFont );
	if ( !font )
		return defaultFont;

	SelectObject( hdc, font );

	TEXTMETRIC tm;
	GetTextMetricsW( hdc, &tm );

	lineHeight = std::max( lineHeight, int( tm.tmHeight ) );

	fonts.push_back( TextFont( font, hdc ) );
	ReleaseDC( NULL, hdc );

	return fonts.size() - 1;
}

void TextStyle::AddFallbackFonts()
{
	// Japanese
	AddFont( L"Meiryo" );

	// Chinese
	AddFont( L"Microsoft JhengHei" );
	AddFont( L"Microsoft YaHei" );

	// Korean
	AddFont( L"Malgun Gothic" );

	// Hebrew
	AddFont( L"Gisha" );
	
	// Thai
	AddFont( L"Leelawadee" );

	// Last ditch
	AddFont( L"SimSun" );
	AddFont( L"Batang" );
	AddFont( L"Arial" );
}
