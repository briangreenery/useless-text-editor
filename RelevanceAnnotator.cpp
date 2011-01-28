// RelevanceAnnotator.cpp

#include "RelevanceAnnotator.h"
#include "TextEditRelevanceLexer.h"
#include "TextStyleRegistry.h"
#include "TextDocument.h"
#include <algorithm>

#undef min
#undef max

RelevanceAnnotator::RelevanceAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_relevanceLexer( doc )
	, m_styleRegistry( styleRegistry )
{
	m_keyword  = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 0,    0,255 ), TextStyle::useDefault ) );
	m_constant = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 128,  0,128 ), TextStyle::useDefault ) );
	m_string   = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 0,  128,128 ), TextStyle::useDefault ) );
}

uint32 RelevanceAnnotator::TokenStyle( RelevanceToken token ) const
{
	switch ( token )
	{
	case RelevanceToken::t_and:
	case RelevanceToken::t_as:
	case RelevanceToken::t_comma:
	case RelevanceToken::t_concat:
	case RelevanceToken::t_containedBy:
	case RelevanceToken::t_contains:
	case RelevanceToken::t_else:
	case RelevanceToken::t_endsWith:
	case RelevanceToken::t_equal:
	case RelevanceToken::t_exists:
	case RelevanceToken::t_greater:
	case RelevanceToken::t_greaterOrEqual:
	case RelevanceToken::t_if:
	case RelevanceToken::t_it:
	case RelevanceToken::t_less:
	case RelevanceToken::t_lessOrEqual:
	case RelevanceToken::t_minus:
	case RelevanceToken::t_mod:
	case RelevanceToken::t_not:
	case RelevanceToken::t_notContainedBy:
	case RelevanceToken::t_notContains:
	case RelevanceToken::t_notEndsWith:
	case RelevanceToken::t_notEqual:
	case RelevanceToken::t_notExists:
	case RelevanceToken::t_notGreater:
	case RelevanceToken::t_notGreaterOrEqual:
	case RelevanceToken::t_notLess:
	case RelevanceToken::t_notLessOrEqual:
	case RelevanceToken::t_notStartsWith:
	case RelevanceToken::t_of:
	case RelevanceToken::t_or:
	case RelevanceToken::t_phraseItem:
	case RelevanceToken::t_phraseItems:
	case RelevanceToken::t_phraseNumber:
	case RelevanceToken::t_plus:
	case RelevanceToken::t_semiColon:
	case RelevanceToken::t_slash:
	case RelevanceToken::t_star:
	case RelevanceToken::t_startsWith:
	case RelevanceToken::t_then:
	case RelevanceToken::t_whose:
		return m_keyword;

	case RelevanceToken::t_string:
	case RelevanceToken::t_unterminatedString:
		return m_string;

	case RelevanceToken::t_number:
	case RelevanceToken::t_false:
	case RelevanceToken::t_true:
		return m_constant;

	case RelevanceToken::t_ignored:
		break;

	case RelevanceToken::t_illegal:
		break;
	}

	return m_styleRegistry.defaultStyleid;
}

void RelevanceAnnotator::TextChanged( TextChange change )
{
	m_relevanceLexer.Reset();
	m_tokens.clear();

	size_t start = 0;
	while ( true )
	{
		RelevanceToken token = m_relevanceLexer.NextToken();

		if ( token == RelevanceToken::t_endOfInput )
			break;

		size_t count = m_relevanceLexer.Position() - start;
		m_tokens.push_back( RelevanceTokenRun( token, start, count ) );
		start += count;
	}
}

void RelevanceAnnotator::SelectionChanged( size_t start, size_t end )
{
}

typedef std::pair<size_t,size_t> TextRange;

struct TokenRunCompare
{
	bool operator()( const RelevanceTokenRun& a, const RelevanceTokenRun&  b ) const { return a.start + a.count  <= b.start; }
	bool operator()( const RelevanceTokenRun& a, const TextRange&          b ) const { return a.start + a.count  <= b.first; }
	bool operator()( const TextRange&         a, const RelevanceTokenRun&  b ) const { return a.first + a.second <= b.start; }
};

void RelevanceAnnotator::GetFonts( TextFontRuns& fonts, size_t start, size_t count )
{
	fonts.push_back( TextFontRun( m_styleRegistry.defaultFontid, count ) );
}

void RelevanceAnnotator::GetStyles( TextStyleRuns& styles, size_t start, size_t count )
{
	typedef std::pair<RelevanceTokenRuns::const_iterator, RelevanceTokenRuns::const_iterator> TokenRange;

	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( RelevanceTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = std::max( start, it->start );
		size_t overlapEnd   = std::min( start + count, it->start + it->count );

		uint32 styleid = TokenStyle( it->token );

		if ( !styles.empty() && styles.back().styleid == styleid )
			styles.back().count += overlapEnd - overlapStart;
		else
			styles.push_back( TextStyleRun( styleid, overlapStart, overlapEnd - overlapStart ) );
	}
}
