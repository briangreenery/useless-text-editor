// TextAnnotator.h

#ifndef TextAnnotator_h
#define TextAnnotator_h

#include "CharChange.h"
#include "TextFontRun.h"
#include "TextStyleRun.h"
#include "CharRange.h"

class TextAnnotator
{
public:
	virtual void TextChanged( CharChange ) = 0;
	virtual void SelectionChanged( size_t start, size_t end ) = 0;

	virtual void GetClasses   ( TextStyleRuns&, size_t start, size_t count ) = 0;
	virtual void GetSquiggles ( CharRanges&,    size_t start, size_t count ) = 0;
	virtual void GetHighlights( CharRanges&,    size_t start, size_t count ) = 0;
};

#endif
