// TextAnnotator.h

#ifndef TextAnnotator_h
#define TextAnnotator_h

#include "TextChange.h"

class TextAnnotator
{
public:
	virtual void TextChanged( TextChange ) = 0;
	virtual void SelectionChanged( size_t start, size_t end ) = 0;

	virtual void GetFonts ( TextFontRuns&,  size_t start, size_t count ) = 0;
	virtual void GetStyles( TextStyleRuns&, size_t start, size_t count ) = 0;
};

#endif