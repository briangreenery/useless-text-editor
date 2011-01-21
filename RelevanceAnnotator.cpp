// RelevanceAnnotator.cpp

#include "RelevanceAnnotator.h"
#include "TextEditRelevanceLexer.h"
#include "TextStyleRegistry.h"
#include <algorithm>

#undef min
#undef max

RelevanceAnnotator::RelevanceAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_relevanceLexer( doc )
	, m_styleRegistry( styleRegistry )
{
	m_keyword  = styleRegistry.AddStyle( TextStyle( styleRegistry.defaultFontid, RGB(0,0,255), RGB(255,255,255) ) );
	m_constant = styleRegistry.AddStyle( TextStyle( styleRegistry.defaultFontid, RGB(128,0,128), RGB(255,255,255) ) );
	m_string   = styleRegistry.AddStyle( TextStyle( styleRegistry.defaultFontid, RGB(0,128,128), RGB(255,255,255) ) );
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
	m_styles.clear();

	size_t position = 0;
	while ( true )
	{
		RelevanceToken token = m_relevanceLexer.NextToken();

		if ( token == RelevanceToken::t_endOfInput )
			break;

		uint32 styleid = TokenStyle( token );
		size_t count = m_relevanceLexer.Position() - position;

		if ( !m_styles.empty() && m_styles.back().styleid == styleid )
			m_styles.back().count += count;
		else
			m_styles.push_back( TextStyleRun( styleid, position, count ) );

		position += count;
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
	typedef std::pair<size_t,size_t> Range;

	struct StyleRunEqual
	{
		bool operator()( const TextStyleRun& a, const TextStyleRun&  b ) const { return a.start + a.count < b.start; }
		bool operator()( const TextStyleRun& a, const Range&         b ) const { return a.start + a.count <= b.first; }
		bool operator()( const Range&        a, const TextStyleRun&  b ) const { return a.first + a.second <= b.start; }
	};

	Range textRange( start, count );
	std::pair<TextStyleRuns::const_iterator, TextStyleRuns::const_iterator> range = std::equal_range( m_styles.begin(), m_styles.end(), textRange, StyleRunEqual() );

	for ( TextStyleRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = std::max( start, it->start );
		size_t overlapEnd   = std::min( start + count, it->start + it->count );

		styles.push_back( TextStyleRun( it->styleid, overlapStart - start, overlapEnd - overlapStart ) );
	}
}
