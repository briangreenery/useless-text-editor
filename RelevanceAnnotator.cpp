// RelevanceAnnotator.cpp

#include "RelevanceAnnotator.h"
#include "TextStyleRegistry.h"
#include "TextDocument.h"
#include "TextDocumentReader.h"
#include "TextRange.h"
#include <algorithm>

RelevanceAnnotator::RelevanceAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_styleRegistry( styleRegistry )
{
	m_default = styleRegistry.ClassID( "default" );
	m_keyword = styleRegistry.ClassID( "keyword.operator.relevance" );
	m_number  = styleRegistry.ClassID( "constant.numeric.relevance" );
	m_string  = styleRegistry.ClassID( "string.quoted.double.relevance" );
}

uint32_t RelevanceAnnotator::TokenStyle( size_t token ) const
{
	switch ( m_tokens[token].token )
	{
	case Relevance::t_concat:
	case Relevance::t_star:
	case Relevance::t_plus:
	case Relevance::t_comma:
	case Relevance::t_minus:
	case Relevance::t_slash:
	case Relevance::t_semi_colon:
	case Relevance::t_or:
	case Relevance::t_not:
	case Relevance::t_equal:
	case Relevance::t_not_equal:
	case Relevance::t_contained_by:
	case Relevance::t_less:
	case Relevance::t_less_or_equal:
	case Relevance::t_greater:
	case Relevance::t_greater_or_equal:
	case Relevance::t_not_greater:
	case Relevance::t_not_greater_or_equal:
	case Relevance::t_not_less:
	case Relevance::t_not_less_or_equal:
	case Relevance::t_not_contained_by:
	case Relevance::t_not_contains:
	case Relevance::t_not_ends_with:
	case Relevance::t_not_starts_with:
	case Relevance::t_ends_with:
	case Relevance::t_starts_with:
	case Relevance::t_exist:
	case Relevance::t_not_exist:
	case Relevance::t_contains:
	case Relevance::t_mod:
	case Relevance::t_whose:
	case Relevance::t_of:
	case Relevance::t_it:
	case Relevance::t_as:
	case Relevance::t_and:
		return m_keyword;

	case Relevance::t_comment:
	case Relevance::e_comment_not_terminated:
		return m_default;

	case Relevance::t_number:
	case Relevance::e_number_too_big:
		return m_number;

	case Relevance::t_string:
	case Relevance::e_string_too_long:
	case Relevance::e_string_not_terminated:
		return m_string;

	case Relevance::t_if:
	case Relevance::t_then:
	case Relevance::t_else:
		return ( std::find( m_matchingTokens.begin(), m_matchingTokens.end(), token ) != m_matchingTokens.end() )
		          ? m_default
		          : m_keyword;
	}

	return m_default;
}

class LexerOutputReceiver : public Relevance::LexerOutput
{
public:
	LexerOutputReceiver( RelevanceTokenRuns& runs )
		: m_runs( runs )
		, m_lastEnd( 0 )
	{}

	virtual void Token( Relevance::LexerToken token, uint32_t pos, uint32_t length )
	{
		if ( token != Relevance::e_bad_percent_sequence )
			AddToken( token, pos, length );
	}

	virtual void Word( Relevance::TextRef, uint32_t pos, uint32_t length )
	{
		AddToken( Relevance::t_word, pos, length );
	}

	virtual void Number( uint64_t value, uint32_t pos, uint32_t length )
	{
		AddToken( Relevance::t_number, pos, length );
	}

	virtual void String( Relevance::TextRef value, uint32_t pos, uint32_t length )
	{
		AddToken( Relevance::t_string, pos, length );
	}

private:
	void AddToken( Relevance::LexerToken token, uint32_t pos, uint32_t length )
	{
		assert( pos >= m_lastEnd );

		if ( pos > m_lastEnd )
			m_runs.push_back( RelevanceTokenRun( Relevance::t_default, m_lastEnd, pos - m_lastEnd ) );

		if ( token != Relevance::t_end_of_input )
			m_runs.push_back( RelevanceTokenRun( token, pos, length ) );

		m_lastEnd = pos + length;
	}

	size_t m_lastEnd;
	RelevanceTokenRuns& m_runs;
};

static inline bool IsAscii( wchar_t c )
{
	return 0 < c && c < 128;
}

void RelevanceAnnotator::TextChanged( TextChange change )
{
	m_tokens.clear();
	m_matchingTokens.clear();

	TextDocumentReader reader( m_doc );

	LexerOutputReceiver receiver( m_tokens );
	Relevance::Lexer lexer( receiver );

	char buffer[512];
	for ( size_t start = 0; start < m_doc.Length(); start += sizeof( buffer ) )
	{
		UTF16Ref text = reader.WeakRange( start, sizeof( buffer ) );

		for ( size_t i = 0; i < text.size(); ++i )
			buffer[i] = IsAscii( text[i] ) ? text[i] : -1;

		lexer.Receive( buffer, text.size() );
	}

	lexer.Finish();
}

void RelevanceAnnotator::SelectionChanged( size_t start, size_t end )
{
	m_matchingTokens.clear();

	if ( start != end || start == 0 )
		return;

	size_t token = TokenAt( start - 1 );
	switch ( m_tokens[token].token )
	{
	case Relevance::t_open_paren:
		MatchOpenParen( token );
		break;

	case Relevance::t_close_paren:
		MatchCloseParen( token );
		break;

	case Relevance::t_if:
		MatchIf( token );
		break;

	case Relevance::t_then:
		MatchThen( token );
		break;

	case Relevance::t_else:
		MatchElse( token );
		break;
	}
}

void RelevanceAnnotator::MatchOpenParen( size_t openParen )
{
	size_t nesting = 0;
	for ( size_t i = openParen + 1; i < m_tokens.size(); ++i )
	{
		Relevance::LexerToken token = m_tokens[i].token;
		if ( token == Relevance::t_close_paren )
		{
			if ( nesting == 0 )
			{
				m_matchingTokens.push_back( openParen );
				m_matchingTokens.push_back( i );
				break;
			}

			nesting--;
		}
		else if ( token == Relevance::t_open_paren )
			nesting++;
	}
}

void RelevanceAnnotator::MatchCloseParen( size_t closeParen )
{
	size_t nesting = 0;
	for ( size_t i = closeParen; i-- > 0; )
	{
		Relevance::LexerToken token = m_tokens[i].token;
		if ( token == Relevance::t_open_paren )
		{
			if ( nesting == 0 )
			{
				m_matchingTokens.push_back( i );
				m_matchingTokens.push_back( closeParen );
				break;
			}

			nesting--;
		}
		else if ( token == Relevance::t_close_paren )
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
		Relevance::LexerToken token = m_tokens[i].token;
		if ( token == Relevance::t_if )
		{
			if ( nesting == 0 )
				return i;

			nesting--;
		}
		else if ( token == Relevance::t_else )
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
		Relevance::LexerToken token = m_tokens[i].token;
		if ( token == Relevance::t_else )
		{
			nesting++;
		}
		else if ( token == Relevance::t_then )
		{
			if ( nesting == 0 )
				return i;
		}
		else if ( token == Relevance::t_if )
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
		Relevance::LexerToken token = m_tokens[i].token;
		if ( token == Relevance::t_if )
		{
			nesting++;
		}
		else if ( token == Relevance::t_then )
		{
			if ( nesting == 0 )
				return i;
		}
		else if ( token == Relevance::t_else )
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
		Relevance::LexerToken token = m_tokens[i].token;
		if ( token == Relevance::t_else )
		{
			if ( nesting == 0 )
				return i;

			nesting--;
		}
		else if ( token == Relevance::t_if )
			nesting++;
	}
	return m_tokens.size();
}

typedef std::pair<RelevanceTokenRuns::const_iterator, RelevanceTokenRuns::const_iterator> TokenRange;

struct TokenRunCompare
{
	bool operator()( const RelevanceTokenRun& a, const RelevanceTokenRun&  b ) const { return a.start + a.count <= b.start; }
	bool operator()( const RelevanceTokenRun& a, const TextRange&          b ) const { return a.start + a.count <= b.start; }
	bool operator()( const TextRange&         a, const RelevanceTokenRun&  b ) const { return a.start + a.count <= b.start; }
};

size_t RelevanceAnnotator::TokenAt( size_t position ) const
{
	TextRange textRange( position, 1 );
	RelevanceTokenRuns::const_iterator it = std::lower_bound( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	assert( it != m_tokens.end() );
	return it - m_tokens.begin();
}

void RelevanceAnnotator::GetClasses( TextStyleRuns& classes, size_t start, size_t count )
{
	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( RelevanceTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = (std::max)( start, it->start );
		size_t overlapEnd   = (std::min)( start + count, it->start + it->count );

		classes.push_back( TextStyleRun( TokenStyle( it - m_tokens.begin() ), overlapStart, overlapEnd - overlapStart ) );
	}
}

void RelevanceAnnotator::GetSquiggles( TextRanges& squiggles, size_t start, size_t count )
{
	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( RelevanceTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = (std::max)( start, it->start );
		size_t overlapEnd   = (std::min)( start + count, it->start + it->count );

		if ( it->token == Relevance::e_word_too_long
		  || it->token == Relevance::e_number_too_big
		  || it->token == Relevance::e_comment_not_terminated
		  || it->token == Relevance::e_bad_percent_sequence
		  || it->token == Relevance::e_string_not_terminated
		  || it->token == Relevance::e_illegal_character )
		{
			if ( !squiggles.empty() && squiggles.back().start + squiggles.back().count == overlapStart )
				squiggles.back().count += overlapEnd - overlapStart;
			else
				squiggles.push_back( TextRange( overlapStart, overlapEnd - overlapStart ) );
		}
	}
}

void RelevanceAnnotator::GetHighlights( TextRanges& highlights, size_t start, size_t count )
{
	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( RelevanceTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = (std::max)( start, it->start );
		size_t overlapEnd   = (std::min)( start + count, it->start + it->count );

		if ( std::find( m_matchingTokens.begin(), m_matchingTokens.end(), it - m_tokens.begin() ) != m_matchingTokens.end() )
		{
			if ( !highlights.empty() && highlights.back().start + highlights.back().count == overlapStart )
				highlights.back().count += overlapEnd - overlapStart;
			else
				highlights.push_back( TextRange( overlapStart, overlapEnd - overlapStart ) );
		}
	}
}
