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

	virtual void GetFonts     ( TextFontRuns&,  size_t start, size_t count );
	virtual void GetStyles    ( TextStyleRuns&, size_t start, size_t count );
	virtual void GetSquiggles ( TextRanges&,    size_t start, size_t count );
	virtual void GetHighlights( TextRanges&,    size_t start, size_t count );

private:
	uint32_t TokenStyle( size_t token ) const;

	const TextDocument& m_doc;
	const TextStyleRegistry& m_styleRegistry;

	std::vector<TextMatePattern> m_patterns;
	TextMateTokenRuns m_tokens;

	uint32_t m_keyword;
	uint32_t m_constant;
	uint32_t m_string;
};

#endif
