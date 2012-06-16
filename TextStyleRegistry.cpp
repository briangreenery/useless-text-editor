// TextStyleRegistry.cpp

#include "TextStyleRegistry.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

TextStyleRegistry::TextStyleRegistry()
	: fontSize( 10 )
	, lineHeight( 0 )
	, avgCharWidth( 0 )
	, tabSize( 0 )
	, annotator( 0 )
	, defaultStyleid( 0 )
	, defaultFontid( 0 )
	, nextFont( 1 )
	, nextStyle( 1 )
{
	gutterColor      = RGB( 236, 236, 236 );
	selectionColor   = RGB( 173, 214, 255 );
	defaultBkColor   = GetSysColor( COLOR_WINDOW );
	defaultTextColor = GetSysColor( COLOR_WINDOWTEXT );

	AddFont( L"Consolas" );
	AddFont( L"Courier New" );
	AddFallbackFonts();

	assert( nextFont > 1 );
	SetDefaultFont( 1 );
}

const TextStyle& TextStyleRegistry::Style( uint32_t styleid ) const
{
	StyleMap::const_iterator it = styles.find( styleid );
	if ( it != styles.end() )
		return it->second;

	return defaultStyle;
}

const TextFont& TextStyleRegistry::Font( uint32_t fontid ) const
{
	FontMap::const_iterator it = fonts.find( fontid );
	if ( it != fonts.end() )
		return *it->second;

	return *defaultFont;
}

void TextStyleRegistry::SetDefaultFont( uint32_t fontid )
{
	FontMap::const_iterator it = fonts.find( fontid );
	if ( it == fonts.end() )
		return;

	TextFontPtr font = CreateFont( it->second->name.c_str() );
	if ( !font )
		return;

	defaultFont = std::move( font );

	HDC hdc = GetDC( NULL );
	SelectObject( hdc, defaultFont->hfont );

	TEXTMETRIC tm;
	GetTextMetricsW( hdc, &tm );

	ReleaseDC( NULL, hdc );

	avgCharWidth = tm.tmAveCharWidth;
	tabSize = 4 * tm.tmAveCharWidth;
	lineHeight = tm.tmHeight + 1; // The "+1" is to give a little more room for the squiggle.
}

uint32_t TextStyleRegistry::AddStyle( const TextStyle& style )
{
	uint32_t styleid = nextStyle++;
	styles.insert( std::make_pair( styleid, style ) );
	return styleid;
}

void TextStyleRegistry::RemoveStyle( uint32_t styleid )
{
	styles.erase( styleid );
}

void TextStyleRegistry::SetDefaultStyle( uint32_t styleid )
{
	StyleMap::const_iterator it = styles.find( styleid );
	if ( it == styles.end() )
		return;

	SetDefaultFont( it->second.fontid );
	defaultBkColor = it->second.bkColor;
	defaultTextColor = it->second.textColor;
}

TextFontPtr TextStyleRegistry::CreateFont( LPCWSTR name )
{
	TextFontPtr font;

	HDC hdc = GetDC( NULL );

	LOGFONT logFont = {};
	logFont.lfHeight = -MulDiv( fontSize, GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
	wcscpy_s( logFont.lfFaceName, name );

	HFONT hfont = CreateFontIndirect( &logFont );
	if ( hfont )
	{
		SelectObject( hdc, hfont );
		font.reset( new TextFont( name, hfont, hdc ) );
	}

	ReleaseDC( NULL, hdc );
	return font;
}

uint32_t TextStyleRegistry::AddFont( LPCWSTR name )
{
	TextFontPtr font = CreateFont( name );

	if ( !font )
		return defaultFontid;

	uint32_t fontid = nextFont++;
	fonts.insert( std::make_pair( fontid, std::move( font ) ) );
	return fontid;
}

void TextStyleRegistry::RemoveFont( uint32_t fontid )
{
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
