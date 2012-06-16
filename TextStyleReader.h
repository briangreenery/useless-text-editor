// TextStyleReader.h

#ifndef TextStyleReader_h
#define TextStyleReader_h

#include "TextStyleRun.h"
#include "TextFontRun.h"
#include "TextRange.h"
#include "ArrayRef.h"

class TextStyleRegistry;
class TextAnnotator;

class TextStyleReader
{
public:
	TextStyleReader( const TextStyleRegistry& styleRegistry );

	ArrayRef<const TextFontRun>  Fonts     ( size_t start, size_t count );
	ArrayRef<const TextStyleRun> Styles    ( size_t start, size_t count );
	ArrayRef<const TextRange>    Squiggles ( size_t start, size_t count );
	ArrayRef<const TextRange>    Highlights( size_t start, size_t count );

private:
	const TextStyleRegistry& m_styleRegistry;
	TextFontRuns m_fonts;
	TextStyleRuns m_styles;
	TextRanges m_squiggles;
	TextRanges m_highlights;
};

#endif
