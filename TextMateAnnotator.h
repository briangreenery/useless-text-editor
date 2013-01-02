// TextMateAnnotator.h

#ifndef TextMateAnnotator_h
#define TextMateAnnotator_h

#include "TextAnnotator.h"
#include "TextMatePattern.h"
#include "TextMateTokenRun.h"
#include "TextDocumentReader.h"
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
	void AddTokensUpTo( size_t offset, uint32_t style );
	void AddTokens( const TextMateRegex* regex, const TextMateCaptures&, uint32_t style, size_t offset );
	void Tokenize( const char* docStart, const char* docEnd );

	const TextDocument& m_doc;
	TextStyleRegistry& m_styleRegistry;
	uint32_t m_defaultStyle;

	TextMateLanguage m_language;
	TextMateTokenRuns m_tokens;
};

#endif
