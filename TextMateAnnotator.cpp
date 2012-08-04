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

TextMatePattern* FindBestMatch( const OnigUChar* textStart,
                                const OnigUChar* textEnd,
                                const OnigUChar* searchStart,
                                TextMatePatterns& patterns )
{
	TextMatePattern* best = 0;

	for ( TextMatePatterns::iterator it = patterns.begin(); it != patterns.end(); ++it )
	{
		// If this is the first time we've searched for this regex in this line
		// or if we're now searching past the start of the last match of this regex
		// then search against this regex.

		if ( searchStart == textStart || ( it->matchStart >= 0 && it->matchStart < searchStart - textStart ) )
			it->matchStart = onig_search( it->regex.get(),
				                            textStart,
				                            textEnd,
				                            searchStart,
				                            textEnd,
				                            it->region.get(),
				                            ONIG_OPTION_SINGLELINE );

		if ( it->matchStart < 0 )
			continue;

		it->matchLength = it->region->end[0] - it->region->beg[0];

		if ( ( best == 0 )
			|| ( it->matchStart < best->matchStart )
			|| ( it->matchStart == best->matchStart && it->matchLength > best->matchLength ) )
			best = &*it;
	}

	return best;
}

const OnigUChar* TextMateAnnotator::Tokenize( size_t offset,
                                              const OnigUChar* textStart,
                                              const OnigUChar* textEnd,
                                              TextMatePatterns& patterns,
                                              TextMatePattern* end )
{
	const TextMatePattern* best;

	for ( const OnigUChar* searchStart = textStart;
	      searchStart < textEnd;
	      searchStart = textStart + best->matchStart + best->matchLength )
	{
		best = FindBestMatch( textStart, textEnd, searchStart, patterns );

		if ( best == 0 )
			break;

		size_t lastEnd = best->matchStart;

		for ( TextMateCaptures::const_iterator it = best->captures.begin(); it != best->captures.end(); ++it )
		{
			if ( it->index > uint32_t( best->region->num_regs ) )
				break;

			size_t start  = best->region->beg[it->index];
			size_t length = best->region->end[it->index] - start;

			if ( length == 0 )
				continue;

			if ( lastEnd != start )
				m_tokens.push_back( TextMateTokenRun( best->classID, offset + lastEnd, start - lastEnd ) );

			m_tokens.push_back( TextMateTokenRun( it->classID, offset + start, length ) );
			lastEnd = start + length;
		}

		if ( lastEnd != best->matchStart + best->matchLength )
			m_tokens.push_back( TextMateTokenRun( best->classID, offset + lastEnd, best->matchStart + best->matchLength - lastEnd ) );

		if ( best == end )
			return best->region->end[0];
	}

	return textEnd;
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
