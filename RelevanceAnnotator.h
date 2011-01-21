// RelevanceAnnotator.h

#ifndef RelevanceAnnotator_h
#define RelevanceAnnotator_h

#include "TextAnnotator.h"
#include "TextEditRelevanceLexer.h"

class TextDocument;
class TextStyleRegistry;

class RelevanceAnnotator : public TextAnnotator
{
public:
	RelevanceAnnotator( const TextDocument&, TextStyleRegistry& );

	virtual void TextChanged( TextChange );
	virtual void SelectionChanged( size_t start, size_t end );

	virtual void GetFonts ( TextFontRuns&,  size_t start, size_t count );
	virtual void GetStyles( TextStyleRuns&, size_t start, size_t count );

private:
	TextStyleRegistry& m_styleRegistry;
	TextEditRelevanceLexer m_relevanceLexer;
	std::vector<RelevanceToken> m_tokens;
};

#endif
