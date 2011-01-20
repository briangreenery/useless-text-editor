// TextStyleRegistry.cpp

#include "TextStyleRegistry.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

#undef min
#undef max

TextStyleRegistry::TextStyleRegistry()
	: fontSize( 10 )
	, lineHeight( 0 )
	, avgCharWidth( 0 )
	, tabSize( 0 )
	, annotator( 0 )
	, defaultStyleid( 0 )
	, defaultFontid( 0 )
	, nextFont( 0 )
	, nextStyle( 0 )
{
	AddFont( L"Consolas" );
	if ( fonts.empty() )
		AddFont( L"Courier New" );

	AddFallbackFonts();
	AddStyle( TextStyle( 0, RGB(0,0,0), RGB(255,255,255) ) );

	gutterColor    = RGB( 236, 236, 236 );
	selectionColor = RGB( 173, 214, 255 );

	SetDefaultStyle( 0 );
}

const TextStyle& TextStyleRegistry::Style( uint32 styleid ) const
{
	StyleMap::const_iterator it = styles.find( styleid );
	if ( it != styles.end() )
		return it->second;

	it = styles.find( defaultStyleid );
	Assert( it != styles.end() );

	return it->second;
}

const TextFont& TextStyleRegistry::Font( uint32 fontid ) const
{
	FontMap::const_iterator it = fonts.find( fontid );
	if ( it != fonts.end() )
		return *it->second;

	it = fonts.find( defaultFontid );
	Assert( it != fonts.end() );

	return *it->second;
}

void TextStyleRegistry::SetDefaultFont( uint32 fontid )
{
	defaultFontid = fontid;

	HDC hdc = GetDC( NULL );

	SelectObject( hdc, fonts[fontid]->hfont );

	TEXTMETRIC tm;
	GetTextMetricsW( hdc, &tm );

	ReleaseDC( NULL, hdc );

	avgCharWidth = tm.tmAveCharWidth;
	tabSize = 4 * tm.tmAveCharWidth;
	lineHeight = tm.tmHeight;
}

uint32 TextStyleRegistry::AddStyle( const TextStyle& style )
{
	uint32 styleid = nextStyle++;
	styles.insert( std::make_pair( styleid, style ) );
	return styleid;
}

void TextStyleRegistry::RemoveStyle( uint32 styleid )
{
	if ( styleid != defaultStyleid )
		styles.erase( styleid );
}

void TextStyleRegistry::SetDefaultStyle( uint32 styleid )
{
	defaultStyleid = styleid;
	SetDefaultFont( Style( defaultStyleid ).fontid );
}

uint32 TextStyleRegistry::AddFont( LPCWSTR name )
{
	HDC hdc = GetDC( NULL );

	LOGFONT logFont = {};
	logFont.lfHeight = -MulDiv( fontSize, GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
	wcscpy_s( logFont.lfFaceName, name );

	HFONT hfont = CreateFontIndirect( &logFont );
	if ( !hfont )
	{
		ReleaseDC( NULL, hdc );
		return defaultFontid;
	}

	uint32 fontid = nextFont++;

	SelectObject( hdc, hfont );
	fonts.insert( std::make_pair( fontid, TextFontPtr( new TextFont( name, hfont, hdc ) ) ) );
	ReleaseDC( NULL, hdc );

	return fontid;
}

void TextStyleRegistry::RemoveFont( uint32 fontid )
{
	if ( fontid != defaultFontid )
		fonts.erase( fontid );
}

void TextStyleRegistry::AddFallbackFonts()
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
