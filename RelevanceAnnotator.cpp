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
	m_keyword  = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 0,    0,255 ),    TextStyle::useDefault ) );
	m_constant = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 128,  0,128 ),    TextStyle::useDefault ) );
	m_string   = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 0,  128,128 ),    TextStyle::useDefault ) );
	m_matching = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, TextStyle::useDefault, RGB( 255,150,50 )     ) );
}

uint32 RelevanceAnnotator::TokenStyle( size_t token ) const
{
	switch ( m_tokens[token].token )
	{
	case RelevanceToken::t_and:
	case RelevanceToken::t_as:
	case RelevanceToken::t_comma:
	case RelevanceToken::t_concat:
	case RelevanceToken::t_containedBy:
	case RelevanceToken::t_contains:
	case RelevanceToken::t_endsWith:
	case RelevanceToken::t_equal:
	case RelevanceToken::t_exists:
	case RelevanceToken::t_greater:
	case RelevanceToken::t_greaterOrEqual:
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
	case RelevanceToken::t_whose:
		return m_keyword;
		
	case RelevanceToken::t_number:
	case RelevanceToken::t_false:
	case RelevanceToken::t_true:
		return m_constant;

	case RelevanceToken::t_string:
	case RelevanceToken::t_unterminatedString:
		return m_string;

	case RelevanceToken::t_if:
	case RelevanceToken::t_then:
	case RelevanceToken::t_else:
		return ( std::find( m_matchingTokens.begin(), m_matchingTokens.end(), token ) != m_matchingTokens.end() )
		          ? m_matching
		          : m_keyword;

	case RelevanceToken::t_openParen:
	case RelevanceToken::t_closeParen:
		return ( std::find( m_matchingTokens.begin(), m_matchingTokens.end(), token ) != m_matchingTokens.end() )
		          ? m_matching
		          : m_styleRegistry.defaultStyleid;
	}

	return m_styleRegistry.defaultStyleid;
}

void RelevanceAnnotator::TextChanged( TextChange change )
{
	m_relevanceLexer.Reset();
	m_tokens.clear();
	m_matchingTokens.clear();

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
	m_matchingTokens.clear();

	if ( start != end || start == 0 )
		return;

	size_t token = TokenAt( start - 1 );
	switch ( m_tokens[token].token )
	{
	case RelevanceToken::t_openParen:
		MatchOpenParen( token );
		break;

	case RelevanceToken::t_closeParen:
		MatchCloseParen( token );
		break;

	case RelevanceToken::t_if:
		MatchIf( token );
		break;

	case RelevanceToken::t_then:
		MatchThen( token );
		break;

	case RelevanceToken::t_else:
		MatchElse( token );
		break;
	}
}

void RelevanceAnnotator::MatchOpenParen( size_t openParen )
{
	size_t nesting = 0;
	for ( size_t i = openParen + 1; i < m_tokens.size(); ++i )
	{
		RelevanceToken token = m_tokens[i].token;
		if ( token == RelevanceToken::t_closeParen )
		{
			if ( nesting == 0 )
			{
				m_matchingTokens.push_back( openParen );
				m_matchingTokens.push_back( i );
				break;
			}

			nesting--;
		}
		else if ( token == RelevanceToken::t_openParen )
			nesting++;
	}
}

void RelevanceAnnotator::MatchCloseParen( size_t closeParen )
{
	size_t nesting = 0;
	for ( size_t i = closeParen; i-- > 0; )
	{
		RelevanceToken token = m_tokens[i].token;
		if ( token == RelevanceToken::t_openParen )
		{
			if ( nesting == 0 )
			{
				m_matchingTokens.push_back( i );
				m_matchingTokens.push_back( closeParen );
				break;
			}

			nesting--;
		}
		else if ( token == RelevanceToken::t_closeParen )
			nesting++;
	}
}

void RelevanceAnnotator::MatchIf( size_t position )
{
	size_t nextThen = NextThen( position );
	size_t nextElse = NextElse( nextThen );

	if ( nextThen < m_tokens.size() || nextElse < m_tokens.size() )
		m_matchingTokens.push_back( position );
	if ( nextThen < m_tokens.size() )
		m_matchingTokens.push_back( nextThen );
	if ( nextElse < m_tokens.size() )
		m_matchingTokens.push_back( nextElse );
}

void RelevanceAnnotator::MatchThen( size_t position )
{
	size_t prevIf   = PrevIf  ( position );
	size_t nextElse = NextElse( position );

	if ( prevIf < m_tokens.size() || nextElse < m_tokens.size() )
		m_matchingTokens.push_back( position );
	if ( prevIf < m_tokens.size() )
		m_matchingTokens.push_back( prevIf );
	if ( nextElse < m_tokens.size() )
		m_matchingTokens.push_back( nextElse );
}

void RelevanceAnnotator::MatchElse( size_t position )
{
	size_t prevThen = PrevThen( position );
	size_t prevIf   = PrevIf  ( prevThen );

	if ( prevIf < m_tokens.size() || prevThen < m_tokens.size() )
		m_matchingTokens.push_back( position );
	if ( prevIf < m_tokens.size() )
		m_matchingTokens.push_back( prevIf );
	if ( prevThen < m_tokens.size() )
		m_matchingTokens.push_back( prevThen );
}

size_t RelevanceAnnotator::PrevIf( size_t position ) const
{
	if ( position >= m_tokens.size() )
		return position;

	size_t nesting = 0;
	for ( size_t i = position; i-- > 0; )
	{
		RelevanceToken token = m_tokens[i].token;
		if ( token == RelevanceToken::t_if )
		{
			if ( nesting == 0 )
				return i;

			nesting--;
		}
		else if ( token == RelevanceToken::t_else )
			nesting++;
	}
	return m_tokens.size();
}

size_t RelevanceAnnotator::PrevThen( size_t position ) const
{
	if ( position >= m_tokens.size() )
		return position;

	size_t nesting = 0;
	for ( size_t i = position; i-- > 0; )
	{
		RelevanceToken token = m_tokens[i].token;
		if ( token == RelevanceToken::t_else )
		{
			nesting++;
		}
		else if ( token == RelevanceToken::t_then )
		{
			if ( nesting == 0 )
				return i;
		}
		else if ( token == RelevanceToken::t_if )
		{
			if ( nesting == 0 )
				break;
			nesting--;
		}
	}
	return m_tokens.size();
}

size_t RelevanceAnnotator::NextThen( size_t position ) const
{
	size_t nesting = 0;
	for ( size_t i = position + 1; i < m_tokens.size(); ++i )
	{
		RelevanceToken::Token token = m_tokens[i].token;
		if ( token == RelevanceToken::t_if )
		{
			nesting++;
		}
		else if ( token == RelevanceToken::t_then )
		{
			if ( nesting == 0 )
				return i;
		}
		else if ( token == RelevanceToken::t_else )
		{
			if ( nesting == 0 )
				break;
			nesting--;
		}
	}
	return m_tokens.size();
}

size_t RelevanceAnnotator::NextElse( size_t position ) const
{
	size_t nesting = 0;
	for ( size_t i = position + 1; i < m_tokens.size(); ++i )
	{
		RelevanceToken::Token token = m_tokens[i].token;
		if ( token == RelevanceToken::t_else )
		{
			if ( nesting == 0 )
				return i;

			nesting--;
		}
		else if ( token == RelevanceToken::t_if )
			nesting++;
	}
	return m_tokens.size();
}

typedef std::pair<size_t,size_t> TextRange;
typedef std::pair<RelevanceTokenRuns::const_iterator, RelevanceTokenRuns::const_iterator> TokenRange;

struct TokenRunCompare
{
	bool operator()( const RelevanceTokenRun& a, const RelevanceTokenRun&  b ) const { return a.start + a.count  <= b.start; }
	bool operator()( const RelevanceTokenRun& a, const TextRange&          b ) const { return a.start + a.count  <= b.first; }
	bool operator()( const TextRange&         a, const RelevanceTokenRun&  b ) const { return a.first + a.second <= b.start; }
};

size_t RelevanceAnnotator::TokenAt( size_t position ) const
{
	TextRange textRange( position, 1 );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	Assert( range.first + 1 == range.second );
	return range.first - m_tokens.begin();
}

void RelevanceAnnotator::GetFonts( TextFontRuns& fonts, size_t start, size_t count )
{
	fonts.push_back( TextFontRun( m_styleRegistry.defaultFontid, count ) );
}

void RelevanceAnnotator::GetStyles( TextStyleRuns& styles, size_t start, size_t count )
{
	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( RelevanceTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = std::max( start, it->start );
		size_t overlapEnd   = std::min( start + count, it->start + it->count );

		uint32 styleid = TokenStyle( it - m_tokens.begin() );

		if ( !styles.empty() && styles.back().styleid == styleid )
			styles.back().count += overlapEnd - overlapStart;
		else
			styles.push_back( TextStyleRun( styleid, overlapStart, overlapEnd - overlapStart ) );
	}
}
