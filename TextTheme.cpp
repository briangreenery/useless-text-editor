// TextTheme.cpp

#include "TextTheme.h"
#include <cassert>

TextTheme::TextTheme()
	: gutterBkColor  ( RGB( 255, 255, 255 ) )
	, gutterLineColor( RGB( 225, 225, 225 ) )
	, gutterTextColor( RGB( 150, 150, 150 ) )
	, selectionColor ( RGB( 173, 214, 255 ) )
	, fontSize( 10 )
	, tabWidth( 4 )
	, m_nextFontID( 1 )
{
	uint32_t id = AddFont( L"Consolas", false, false );

	if ( id == 0 )
		id = AddFont( L"Courier New", false, false );

	SetStyle( "default", TextStyle( id, GetSysColor( COLOR_WINDOWTEXT ), GetSysColor( COLOR_WINDOW ) ) );
}

uint32_t TextTheme::AddFont( const std::wstring& name, bool bold, bool italic )
{
	try
	{
		m_fonts[m_nextFontID].reset( new TextFont( name, fontSize, bold, italic ) );
	}
	catch (...)
	{
		return 0;
	}

	return m_nextFontID++;
}

const TextFont& TextTheme::Font( uint32_t fontID ) const
{
	FontMap::const_iterator it = m_fonts.find( fontID );

	if ( it != m_fonts.end() )
		return *it->second;

	it = m_fonts.find( Style( "default" ).fontid );
	assert( it != m_fonts.end() );
	return *it->second;
}

void TextTheme::SetStyle( const std::string& className, const TextStyle& style )
{
	m_styles[className] = style;
}

static std::string ParentClass( const std::string& className )
{
	std::string::size_type i =  className.rfind( '.' );

	if ( i == std::string::npos )
		return std::string();

	return className.substr( 0, i );
}

const TextStyle& TextTheme::Style( std::string className ) const
{
	while ( !className.empty() )
	{
		StyleMap::const_iterator it = m_styles.find( className );

		if ( it != m_styles.end() )
			return it->second;

		className = ParentClass( className );
	}

	StyleMap::const_iterator it = m_styles.find( "default" );
	assert( it != m_styles.end() );
	return it->second;
}
