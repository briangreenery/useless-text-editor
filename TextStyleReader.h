// TextStyleReader.h

#ifndef TextStyleReader_h
#define TextStyleReader_h

#include "TextStyleRun.h"
#include "TextFontRun.h"
#include "TextRange.h"
#include "ArrayOf.h"

class TextStyleRegistry;
class TextAnnotator;

class TextStyleReader
{
public:
	TextStyleReader( const TextStyleRegistry& styleRegistry );

	ArrayOf<const TextFontRun>  Fonts    ( size_t start, size_t count );
	ArrayOf<const TextStyleRun> Styles   ( size_t start, size_t count );
	ArrayOf<const TextRange>    Squiggles( size_t start, size_t count );

private:
	const TextStyleRegistry& m_styleRegistry;
	TextFontRuns m_fonts;
	TextStyleRuns m_styles;
	TextRanges m_squiggles;
};

#endif
