// TextStyleReader.cpp

#include "TextStyleReader.h"
#include "TextAnnotator.h"
#include "TextStyleRegistry.h"

TextStyleReader::TextStyleReader( const TextStyleRegistry& styleRegistry )
	: m_styleRegistry( styleRegistry )
{
}

ArrayOf<const TextFontRun> TextStyleReader::Fonts( size_t start, size_t count )
{
	m_fonts.clear();

	if ( m_styleRegistry.annotator )
	{
		m_styleRegistry.annotator->GetFonts( m_fonts, start, count );
	}
	else
	{
		m_fonts.push_back( TextFontRun( m_styleRegistry.defaultFontid, count ) );
	}

	return ArrayOf<const TextFontRun>( &m_fonts.front(), &m_fonts.back() + 1 );
}

ArrayOf<const TextStyleRun> TextStyleReader::Styles( size_t start, size_t count )
{
	m_styles.clear();

	if ( m_styleRegistry.annotator )
	{
		m_styleRegistry.annotator->GetStyles( m_styles, start, count );
	}
	else
	{
		m_styles.push_back( TextStyleRun( m_styleRegistry.defaultStyleid, start, count ) );
	}

	return ArrayOf<const TextStyleRun>( &m_styles.front(), &m_styles.back() + 1 );
}

ArrayOf<const TextRange> TextStyleReader::Squiggles( size_t start, size_t count )
{
	m_squiggles.clear();

	if ( m_styleRegistry.annotator )
		m_styleRegistry.annotator->GetSquiggles( m_squiggles, start, count );

	if ( m_squiggles.empty() )
		return ArrayOf<const TextRange>();

	return ArrayOf<const TextRange>( &m_squiggles.front(), &m_squiggles.back() + 1 );
}
