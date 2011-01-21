// RelevanceAnnotator.cpp

#include "RelevanceAnnotator.h"
#include "TextEditRelevanceLexer.h"
#include "TextStyleRegistry.h"

RelevanceAnnotator::RelevanceAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_relevanceLexer( doc )
	, m_styleRegistry( styleRegistry )
{
}

void RelevanceAnnotator::TextChanged( TextChange change )
{
	m_relevanceLexer.Reset();
	m_tokens.clear();

	while ( true )
	{
		RelevanceToken token = m_relevanceLexer.NextToken();

		if ( token == RelevanceToken::t_endOfInput )
			break;

		m_tokens.push_back( token );
	}
}

void RelevanceAnnotator::SelectionChanged( size_t start, size_t end )
{
}

void RelevanceAnnotator::GetFonts( TextFontRuns& fonts, size_t start, size_t count )
{
	fonts.push_back( TextFontRun( m_styleRegistry.defaultFontid, count ) );
}

void RelevanceAnnotator::GetStyles( TextStyleRuns& styles, size_t start, size_t count )
{
	styles.push_back( TextStyleRun( m_styleRegistry.defaultStyleid, start, count ) );
}
