// TextMateAnnotator.h

#ifndef TextMateAnnotator_h
#define TextMateAnnotator_h

#include "TextAnnotator.h"
#include "TextMatePattern.h"
#include "TextMateTokenRun.h"
#include <vector>

class Document;
class TextStyleRegistry;

class TextMateAnnotator : public TextAnnotator
{
public:
	TextMateAnnotator( const Document&, TextStyleRegistry& );

	void SetLanguageFile( const char* path );

	virtual void TextChanged( CharChange );
	virtual void SelectionChanged( size_t start, size_t end );

	virtual void GetClasses   ( TextStyleRuns&, size_t start, size_t count );
	virtual void GetSquiggles ( CharRanges&,    size_t start, size_t count );
	virtual void GetHighlights( CharRanges&,    size_t start, size_t count );

private:
	void AddTokensUpTo( size_t offset, uint32_t style );
	void AddTokens( const TextMateRegex* regex, const TextMateCaptures&, uint32_t style, size_t offset );
	void Tokenize( const char* docStart, const char* docEnd );

	const Document& m_doc;
	TextStyleRegistry& m_styleRegistry;
	uint32_t m_defaultStyle;

	TextMateLanguage m_language;
	TextMateTokenRuns m_tokens;
};

#endif
