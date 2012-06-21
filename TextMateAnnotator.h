// TextMateAnnotator.h

#ifndef TextMateAnnotator_h
#define TextMateAnnotator_h

#include "TextAnnotator.h"
#include "TextMatePattern.h"
#include "TextMateTokenRun.h"
#include "UTF16Ref.h"
#include <vector>

class TextDocument;
class TextStyleRegistry;

class TextMateAnnotator : public TextAnnotator
{
public:
	TextMateAnnotator( const TextDocument&, TextStyleRegistry& );

	void SetLanguageFile( const char* path );

	virtual void TextChanged( TextChange );
	virtual void SelectionChanged( size_t start, size_t end );

	virtual void GetClasses   ( TextStyleRuns&, size_t start, size_t count );
	virtual void GetSquiggles ( TextRanges&,    size_t start, size_t count );
	virtual void GetHighlights( TextRanges&,    size_t start, size_t count );

private:
	void TokenizeLine( size_t offset, size_t lineEnd, std::string line );

	const TextDocument& m_doc;
	TextStyleRegistry& m_styleRegistry;

	TextMatePatterns m_patterns;
	TextMateTokenRuns m_tokens;
};

#endif
