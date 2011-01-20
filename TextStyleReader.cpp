// TextStyleReader.cpp

#include "TextStyleReader.h"
#include "TextAnnotator.h"
#include "TextStyleRegistry.h"

TextStyleReader::TextStyleReader( const TextStyleRegistry& styleRegistry, TextAnnotator* annotator )
	: m_styleRegistry( styleRegistry )
	, m_annotator( annotator )
{
}

ArrayOf<const TextFontRun> TextStyleReader::Fonts( size_t start, size_t count )
{
	m_fonts.clear();

	if ( m_annotator )
	{
		m_annotator->GetFonts( m_fonts, start, count );
	}
	else
	{
		//for ( size_t i = 0; i < count; ++i )
		//	m_fonts.push_back( TextFontRun( ( i / 3 ) % 2, 1 ) );

		m_fonts.push_back( TextFontRun( m_styleRegistry.defaultFontid, count ) );
	}

	return ArrayOf<const TextFontRun>( &m_fonts.front(), &m_fonts.back() + 1 );
}

ArrayOf<const TextStyleRun> TextStyleReader::Styles( size_t start, size_t count )
{
	m_styles.clear();

	if ( m_annotator )
	{
		m_annotator->GetStyles( m_styles, start, count );
	}
	else
	{
		for ( size_t i = 0; i < count; ++i )
			m_styles.push_back( TextStyleRun( ( i / 3 ) % 2, start + i, 1 ) );
		
		//m_styles.push_back( TextStyleRun( m_styleRegistry.defaultStyleid, start, count ) );
	}

	return ArrayOf<const TextStyleRun>( &m_styles.front(), &m_styles.back() + 1 );
}
