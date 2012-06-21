// TextMateAnnotator.cpp

#include "TextMateAnnotator.h"
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

void TextMateAnnotator::TokenizeLine( size_t offset, AsciiRef text )
{
	const OnigUChar* textStart = reinterpret_cast<const uint8_t*>( text.begin() );
	const OnigUChar* textEnd   = textStart + text.size();

	const TextMatePattern* bestPattern;
	size_t bestStart;
	size_t bestLength;

	for ( const OnigUChar* searchStart = textStart; searchStart < textEnd; searchStart += bestStart + bestLength )
	{
		bestPattern = 0;
		bestStart   = text.size();
		bestLength  = 0;

		for ( TextMatePatterns::const_iterator it = m_patterns.begin(); it != m_patterns.end(); ++it )
		{
			int matchPos = onig_search( it->regex.get(),
			                            textStart,
			                            textEnd,
			                            searchStart,
			                            textEnd,
			                            it->region.get(),
			                            ONIG_OPTION_NONE );

			if ( matchPos >= 0 )
			{
				size_t start = matchPos;
				if ( start > bestStart )
					continue;

				size_t length = it->region->end[0] - it->region->beg[0];
				if ( start == bestStart && length < bestLength )
					continue;

				bestPattern = &*it;
				bestStart   = start;
				bestLength  = length;
			}
		}

		if ( bestPattern == 0 )
			break;

		if ( bestPattern->captures.empty() )
		{
			m_tokens.push_back( TextMateTokenRun( bestPattern->classID, offset + bestStart, bestLength ) );
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

				m_tokens.push_back( TextMateTokenRun( it->classID, offset + start, length ) );
			}
		}
	}
}

void TextMateAnnotator::TextChanged( TextChange )
{
	m_tokens.clear();

	TextDocumentReader reader( m_doc );

	AsciiRef text = reader.AsciiRange( 0, m_doc.Length() );

	const char* lineStart = text.begin();;

	while ( true )
	{
		const char* newline = std::find( lineStart, text.end(), 0x0A );
		TokenizeLine( lineStart - text.begin(), AsciiRef( lineStart, newline ) );

		if ( newline == text.end() )
			break;

		lineStart = newline + 1;
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
