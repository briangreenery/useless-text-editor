// TextStyleReader.cpp

#include "TextStyleReader.h"
#include "TextAnnotator.h"
#include "TextStyleRegistry.h"

TextStyleReader::TextStyleReader( TextStyleRegistry& styleRegistry )
	: m_styleRegistry( styleRegistry )
	, m_defaultClassID( styleRegistry.ClassID( "default" ) )
{
}

void TextStyleReader::AddFont( uint32_t classID, size_t count )
{
	uint32_t fontID = m_styleRegistry.Style( classID ).fontid;

	if ( !m_fonts.empty() && m_fonts.back().fontid == fontID )
		m_fonts.back().count += count;
	else
		m_fonts.push_back( TextFontRun( fontID, count ) );
}

ArrayRef<const TextFontRun> TextStyleReader::Fonts( size_t start, size_t count )
{
	m_fonts.clear();

	m_annotatorClasses.clear();

	if ( m_styleRegistry.Annotator() )
		m_styleRegistry.Annotator()->GetClasses( m_annotatorClasses, start, count );

	size_t lastEnd = start;

	for ( TextStyleRuns::const_iterator it = m_annotatorClasses.begin(); it != m_annotatorClasses.end(); ++it )
	{
		if ( lastEnd != it->start )
			AddFont( m_defaultClassID, it->start - lastEnd );

		AddFont( it->styleid, it->count );
		lastEnd = it->start + it->count;
	}

	if ( lastEnd != start + count )
		AddFont( m_defaultClassID, start + count - lastEnd );

	return ArrayRef<const TextFontRun>( &m_fonts.front(), &m_fonts.back() + 1 );
}

void TextStyleReader::AddClass( uint32_t classID, size_t start, size_t count )
{
	if ( !m_classes.empty() && m_classes.back().styleid == classID )
		m_classes.back().count += count;
	else
		m_classes.push_back( TextStyleRun( classID, start, count ) );
}

ArrayRef<const TextStyleRun> TextStyleReader::Classes( size_t start, size_t count )
{
	m_classes.clear();

	m_annotatorClasses.clear();

	if ( m_styleRegistry.Annotator() )
		m_styleRegistry.Annotator()->GetClasses( m_annotatorClasses, start, count );

	size_t lastEnd = start;

	for ( TextStyleRuns::const_iterator it = m_annotatorClasses.begin(); it != m_annotatorClasses.end(); ++it )
	{
		if ( lastEnd != it->start )
			AddClass( m_defaultClassID, lastEnd, it->start - lastEnd );

		AddClass( it->styleid, it->start, it->count );
		lastEnd = it->start + it->count;
	}

	if ( lastEnd != start + count )
		AddClass( m_defaultClassID, lastEnd, start + count - lastEnd );

	return ArrayRef<const TextStyleRun>( &m_classes.front(), &m_classes.back() + 1 );
}

ArrayRef<const TextRange> TextStyleReader::Squiggles( size_t start, size_t count )
{
	m_squiggles.clear();

	if ( m_styleRegistry.Annotator() )
		m_styleRegistry.Annotator()->GetSquiggles( m_squiggles, start, count );

	if ( m_squiggles.empty() )
		return ArrayRef<const TextRange>();

	return ArrayRef<const TextRange>( &m_squiggles.front(), &m_squiggles.back() + 1 );
}

ArrayRef<const TextRange> TextStyleReader::Highlights( size_t start, size_t count )
{
	m_highlights.clear();

	if ( m_styleRegistry.Annotator() )
		m_styleRegistry.Annotator()->GetHighlights( m_highlights, start, count );

	if ( m_highlights.empty() )
		return ArrayRef<const TextRange>();

	return ArrayRef<const TextRange>( &m_highlights.front(), &m_highlights.back() + 1 );
}
