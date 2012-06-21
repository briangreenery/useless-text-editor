// TextMateAnnotator.cpp

#include "TextMateAnnotator.h"
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
		const TextMatePattern* bestPattern = 0;
		size_t bestStart  = line.size();
		size_t bestLength = 0;

		for ( TextMatePatterns::const_iterator it = m_patterns.begin(); it != m_patterns.end(); ++it )
		{
			const OnigUChar* start = reinterpret_cast<const unsigned char*>( line.c_str() );
			const OnigUChar* end   = start + line.size();

			int matchPos = onig_search( it->regex.get(), start, end, start, end, it->region.get(), ONIG_OPTION_NONE );

			if ( matchPos >= 0 )
			{
				size_t start = matchPos;
				if ( start > bestStart )
					continue;

				size_t length = it->region->end[0] - it->region->beg[0];
				if ( start == bestStart && length < bestLength )
					continue;

				bestPattern = &*it;
				bestStart = start;
				bestLength = length;
			}
		}

		if ( bestPattern == 0 )
			break;

		size_t nextStart = bestStart + bestLength;
		bestStart += offset;

		if ( bestPattern->captures.empty() )
		{
			m_tokens.push_back( TextMateTokenRun( bestPattern->classID, bestStart, bestLength ) );
		}
		else
		{
			for ( TextMateCaptures::const_iterator it = bestPattern->captures.begin(); it != bestPattern->captures.end(); ++it )
			{
				if ( it->index > uint32_t( bestPattern->region->num_regs ) )
					break;

				size_t start  = bestPattern->region->beg[it->index];
				size_t length = bestPattern->region->end[it->index] - start;

				if ( length == 0 )
					continue;

				m_tokens.push_back( TextMateTokenRun( it->classID, start + offset, length ) );
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
