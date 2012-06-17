// TextStyleReader.cpp

#include "TextStyleReader.h"
#include "TextAnnotator.h"
#include "TextStyleRegistry.h"

TextStyleReader::TextStyleReader( const TextStyleRegistry& styleRegistry )
	: m_styleRegistry( styleRegistry )
{
}

ArrayRef<const TextFontRun> TextStyleReader::Fonts( size_t start, size_t count )
{
	m_fonts.clear();

	if ( m_styleRegistry.annotator )
		m_styleRegistry.annotator->GetFonts( m_fonts, start, count );

	if ( m_fonts.empty() )
		m_fonts.push_back( TextFontRun( m_styleRegistry.defaultFontid, count ) );

	return ArrayRef<const TextFontRun>( &m_fonts.front(), &m_fonts.back() + 1 );
}

ArrayRef<const TextStyleRun> TextStyleReader::Styles( size_t start, size_t count )
{
	m_styles.clear();

	if ( m_styleRegistry.annotator )
		m_styleRegistry.annotator->GetStyles( m_styles, start, count );

	if ( m_styles.empty() )
		m_styles.push_back( TextStyleRun( m_styleRegistry.defaultStyleid, start, count ) );

	return ArrayRef<const TextStyleRun>( &m_styles.front(), &m_styles.back() + 1 );
}

ArrayRef<const TextRange> TextStyleReader::Squiggles( size_t start, size_t count )
{
	m_squiggles.clear();

	if ( m_styleRegistry.annotator )
		m_styleRegistry.annotator->GetSquiggles( m_squiggles, start, count );

	if ( m_squiggles.empty() )
		return ArrayRef<const TextRange>();

	return ArrayRef<const TextRange>( &m_squiggles.front(), &m_squiggles.back() + 1 );
}

ArrayRef<const TextRange> TextStyleReader::Highlights( size_t start, size_t count )
{
	m_highlights.clear();

	if ( m_styleRegistry.annotator )
		m_styleRegistry.annotator->GetHighlights( m_highlights, start, count );

	if ( m_highlights.empty() )
		return ArrayRef<const TextRange>();

	return ArrayRef<const TextRange>( &m_highlights.front(), &m_highlights.back() + 1 );
}
