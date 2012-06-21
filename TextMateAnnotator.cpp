// TextMateAnnotator.cpp

#include "TextMateAnnotator.h"
#include "rapidxml\rapidxml.hpp"
#include "TextDocumentReader.h"
#include "TextDocument.h"
#include "TextStyleRegistry.h"
#include <string>

TextMateAnnotator::TextMateAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_styleRegistry( styleRegistry )
{
}

void TextMateAnnotator::SetLanguageFile( const char* path )
{
	m_patterns = ReadTextMateFile( path, m_styleRegistry );
}

static inline bool IsAscii( wchar_t c )
{
	return 0 < c && c < 128;
}

void TextMateAnnotator::TokenizeLine( size_t offset, size_t lineEnd, std::string line )
{
	while ( !line.empty() )
	{
		size_t best       = m_patterns.size();
		size_t bestStart  = line.size();
		size_t bestLength = 0;

		for ( size_t i = 0; i < m_patterns.size(); ++i )
		{
			if ( std::regex_search( line.c_str(), line.c_str() + line.size(), m_patterns[i].match, m_patterns[i].regex ) )
			{
				size_t start = m_patterns[i].match.position();
				if ( start > bestStart )
					continue;

				size_t length = m_patterns[i].match.length();
				if ( start == bestStart && length < bestLength )
					continue;

				best = i;
				bestStart = start;
				bestLength = length;
			}
		}

		if ( best == m_patterns.size() )
			break;

		size_t nextStart = bestStart + bestLength;
		bestStart += offset;

		if ( m_patterns[best].captures.empty() )
		{
			m_tokens.push_back( TextMateTokenRun( m_patterns[best].classID, bestStart, bestLength ) );
		}
		else
		{
			size_t lastEnd = offset;

			const TextMatePattern& pattern = m_patterns[best];

			for ( size_t i = 0; i < pattern.captures.size(); ++i )
			{
				if ( pattern.captures[i].index > pattern.match.size() )
					break;

				size_t start  = pattern.match[pattern.captures[i].index].first - line.c_str() + offset;
				size_t length = pattern.match[pattern.captures[i].index].length();

				if ( length == 0 )
					continue;

				m_tokens.push_back( TextMateTokenRun( pattern.captures[i].classID, start, length ) );
				lastEnd = start + length;
			}
		}

		line = line.substr( nextStart );
		offset = bestStart + bestLength;
	}
}

void TextMateAnnotator::TextChanged( TextChange )
{
	m_tokens.clear();

	TextDocumentReader reader( m_doc );

	std::string text;
	text.resize( m_doc.Length() );

	UTF16Ref text16 = reader.WeakRange( 0, m_doc.Length() );

	for ( size_t i = 0; i < text16.size(); ++i )
		text[i] = IsAscii( text16[i] ) ? text16[i] : -1;

	size_t offset = 0;

	while ( true )
	{
		std::string::const_iterator lineBreak = std::find( text.begin() + offset, text.end(), 0x0A );

		std::string line = std::string( text.begin() + offset, lineBreak );
		TokenizeLine( offset, offset + line.size(), line );

		if ( lineBreak == text.end() )
			break;

		offset += line.size() + 1;
	}
}

void TextMateAnnotator::SelectionChanged( size_t start, size_t end )
{
}

typedef std::pair<TextMateTokenRuns::const_iterator, TextMateTokenRuns::const_iterator> TokenRange;

struct TokenRunCompare
{
	bool operator()( const TextMateTokenRun& a, const TextMateTokenRun&  b ) const { return a.start + a.count <= b.start; }
	bool operator()( const TextMateTokenRun& a, const TextRange&         b ) const { return a.start + a.count <= b.start; }
	bool operator()( const TextRange&        a, const TextMateTokenRun&  b ) const { return a.start + a.count <= b.start; }
};

void TextMateAnnotator::GetClasses( TextStyleRuns& styles, size_t start, size_t count )
{
	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( TextMateTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = (std::max)( start, it->start );
		size_t overlapEnd   = (std::min)( start + count, it->start + it->count );

		styles.push_back( TextStyleRun( it->classID, overlapStart, overlapEnd - overlapStart ) );
	}
}

void TextMateAnnotator::GetSquiggles( TextRanges&, size_t start, size_t count )
{
}

void TextMateAnnotator::GetHighlights( TextRanges&, size_t start, size_t count )
{
}
