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
	TextStyleReader( TextStyleRegistry& styleRegistry );

	ArrayRef<const TextFontRun>  Fonts     ( size_t start, size_t count );
	ArrayRef<const TextStyleRun> Classes   ( size_t start, size_t count );
	ArrayRef<const TextRange>    Squiggles ( size_t start, size_t count );
	ArrayRef<const TextRange>    Highlights( size_t start, size_t count );

private:
	void AddFont( uint32_t classID, size_t count );
	void AddClass( uint32_t classID, size_t start, size_t count );

	const TextStyleRegistry& m_styleRegistry;
	uint32_t m_defaultClassID;

	TextFontRuns m_fonts;
	TextStyleRuns m_classes;
	TextRanges m_squiggles;
	TextRanges m_highlights;
};

#endif
