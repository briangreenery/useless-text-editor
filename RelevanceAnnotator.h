// RelevanceAnnotator.h

#ifndef RelevanceAnnotator_h
#define RelevanceAnnotator_h

#include "TextAnnotator.h"
#include "RelevanceLexer.h"
#include "RelevanceTokenRun.h"
#include "TextStyleRun.h"

class TextDocument;
class TextStyleRegistry;

class RelevanceAnnotator : public TextAnnotator
{
public:
	RelevanceAnnotator( const TextDocument&, TextStyleRegistry& );

	virtual void TextChanged( TextChange );
	virtual void SelectionChanged( size_t start, size_t end );

	virtual void GetClasses   ( TextStyleRuns&, size_t start, size_t count );
	virtual void GetSquiggles ( TextRanges&,    size_t start, size_t count );
	virtual void GetHighlights( TextRanges&,    size_t start, size_t count );

private:
	size_t TokenAt( size_t position ) const;
	uint32_t TokenStyle( size_t token ) const;

	void MatchOpenParen ( size_t position );
	void MatchCloseParen( size_t position );
	void MatchIf        ( size_t position );
	void MatchThen      ( size_t position );
	void MatchElse      ( size_t position );

	size_t PrevIf  ( size_t position ) const;
	size_t PrevThen( size_t position ) const;
	size_t NextThen( size_t position ) const;
	size_t NextElse( size_t position ) const;

	const TextDocument& m_doc;
	TextStyleRegistry& m_styleRegistry;

	uint32_t m_keyword;
	uint32_t m_string;
	uint32_t m_number;
	uint32_t m_default;

	std::vector<size_t> m_matchingTokens;
	RelevanceTokenRuns m_tokens;
};

#endif
