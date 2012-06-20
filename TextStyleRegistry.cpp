// TextStyleRegistry.cpp

#include "TextStyleRegistry.h"
#include <algorithm>
#include <cassert>

TextStyleRegistry::TextStyleRegistry()
	: m_annotator( 0 )
	, m_nextClassID( 1 )
{
	SetTheme( TextTheme() );
}

void TextStyleRegistry::SetAnnotator( TextAnnotator* annotator )
{
	m_annotator = annotator;
}

TextAnnotator* TextStyleRegistry::Annotator() const
{
	return m_annotator;
}

void TextStyleRegistry::SetTheme( const TextTheme& theme )
{
	m_theme = theme;

	m_defaultStyle = m_theme.Style( "default" );

	UpdateFontMetrics();

	for ( ClassMap::const_iterator it = m_classes.begin(); it != m_classes.end(); ++it )
		m_styles[it->second] = m_theme.Style( it->first );

	ResetFallbackFonts();
}

const TextTheme& TextStyleRegistry::Theme() const
{
	return m_theme;
}

uint32_t TextStyleRegistry::ClassID( const std::string& className )
{
	ClassMap::const_iterator it = m_classes.find( className );

	if ( it != m_classes.end() )
		return it->second;

	m_classes[className] = m_nextClassID;
	m_styles[m_nextClassID] = m_theme.Style( className );
	return m_nextClassID++;
}

const TextStyle& TextStyleRegistry::Style( uint32_t classID ) const
{
	StyleMap::const_iterator it = m_styles.find( classID );
	if ( it != m_styles.end() )
		return it->second;

	return m_defaultStyle;
}

const TextFont& TextStyleRegistry::Font( uint32_t fontID ) const
{
	return m_theme.Font( fontID );
}

const TextStyle& TextStyleRegistry::DefaultStyle() const
{
	return m_defaultStyle;
}

const TextFont& TextStyleRegistry::DefaultFont() const
{
	return m_theme.Font( m_defaultStyle.fontid );
}

void TextStyleRegistry::UpdateFontMetrics()
{
	HDC hdc = GetDC( NULL );
	SelectObject( hdc, DefaultFont().hfont );

	TEXTMETRIC tm;
	GetTextMetricsW( hdc, &tm );

	ReleaseDC( NULL, hdc );

	m_avgCharWidth = tm.tmAveCharWidth;
	m_lineHeight = tm.tmHeight + 1; // The "+1" is to give a little more room for the red squiggle underline.
}

uint32_t TextStyleRegistry::AddFallbackFont( const std::wstring& name )
{
	uint32_t id = m_theme.AddFont( name, false, false );

	if ( id == 0 )
		return m_defaultStyle.fontid;

	m_fallbackFonts.insert( id );
	return id;
}

void TextStyleRegistry::RemoveFallbackFont( uint32_t fontID )
{
	m_fallbackFonts.erase( fontID );
}

const std::set<uint32_t>& TextStyleRegistry::FallbackFonts() const
{
	return m_fallbackFonts;
}

void TextStyleRegistry::ResetFallbackFonts()
{
	m_fallbackFonts.clear();

	// Japanese
	AddFallbackFont( L"Meiryo" );

	// Chinese
	AddFallbackFont( L"Microsoft JhengHei" );
	AddFallbackFont( L"Microsoft YaHei" );

	// Korean
	AddFallbackFont( L"Malgun Gothic" );

	// Hebrew
	AddFallbackFont( L"Gisha" );
	
	// Thai
	AddFallbackFont( L"Leelawadee" );

	// Last ditch
	AddFallbackFont( L"SimSun" );
	AddFallbackFont( L"Batang" );
	AddFallbackFont( L"Arial" );
}
