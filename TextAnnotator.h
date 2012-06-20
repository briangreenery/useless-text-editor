// TextAnnotator.h

#ifndef TextAnnotator_h
#define TextAnnotator_h

#include "TextChange.h"
#include "TextFontRun.h"
#include "TextStyleRun.h"
#include "TextRange.h"

class TextAnnotator
{
public:
	virtual void TextChanged( TextChange ) = 0;
	virtual void SelectionChanged( size_t start, size_t end ) = 0;

	virtual void GetClasses   ( TextStyleRuns&, size_t start, size_t count ) = 0;
	virtual void GetSquiggles ( TextRanges&,    size_t start, size_t count ) = 0;
	virtual void GetHighlights( TextRanges&,    size_t start, size_t count ) = 0;
};

#endif
