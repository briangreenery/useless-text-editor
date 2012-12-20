// TextMateAnnotator.cpp

#include "TextMateAnnotator.h"
#include "TextDocument.h"
#include "TextStyleRegistry.h"
#include <string>

TextMateAnnotator::TextMateAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_styleRegistry( styleRegistry )
	, m_defaultStyle( styleRegistry.ClassID( "default" ) )
{
}

void TextMateAnnotator::SetLanguageFile( const char* path )
{
	m_language = ReadTextMateFile( path, m_styleRegistry );
}

static TextMateBestMatch FindBestRegexMatch( const char* textStart,
                                             const char* textEnd,
                                             const char* searchStart,
                                             const std::vector<TextMateRegex*>& regexes )
{
	TextMateBestMatch bestMatch;

	for ( int32_t i = 0, size = regexes.size(); i < size; ++i )
	{
		TextMateRegex* regex = regexes[i];

		if ( searchStart == textStart || ( regex->matchStart >= 0 && regex->matchStart < searchStart - textStart ) )
			regex->matchStart = onig_search( regex->regex.get(),
			                                 reinterpret_cast<const OnigUChar*>( textStart ),
			                                 reinterpret_cast<const OnigUChar*>( textEnd ),
			                                 reinterpret_cast<const OnigUChar*>( searchStart ),
			                                 reinterpret_cast<const OnigUChar*>( textEnd ),
			                                 regex->region.get(),
			                                 ONIG_OPTION_NONE );

		if ( regex->matchStart < 0 )
			continue;

		regex->matchLength = regex->region->end[0] - regex->region->beg[0];

		if ( !bestMatch.IsBetterThan( regex->matchStart, regex->matchLength ) )
		{
			bestMatch.index  = i;
			bestMatch.start  = regex->matchStart;
			bestMatch.length = regex->matchLength;
		}
	}

	return bestMatch;
}

void TextMateAnnotator::AddTokens( size_t startOffset, uint32_t blockStyle, uint32_t regexStyle, const TextMateRegex* regex )
{
	size_t lastEnd = m_tokens.empty() ? 0 : m_tokens.back().start + m_tokens.back().count;

	if ( lastEnd != startOffset + regex->matchStart )
		m_tokens.push_back( TextMateTokenRun( blockStyle, lastEnd, startOffset + regex->matchStart - lastEnd ) );

	lastEnd = regex->matchStart;

	for ( auto it = regex->captures.begin(); it != regex->captures.end(); ++it )
	{
		if ( it->index > uint32_t( regex->region->num_regs ) )
			break;

		size_t start  = regex->region->beg[it->index];
		size_t length = regex->region->end[it->index] - start;

		if ( length == 0 )
			continue;

		if ( lastEnd != start )
			m_tokens.push_back( TextMateTokenRun( regexStyle, startOffset + lastEnd, start - lastEnd ) );

		m_tokens.push_back( TextMateTokenRun( it->style, startOffset + start, length ) );
		lastEnd = start + length;
	}

	if ( lastEnd != regex->matchStart + regex->matchLength )
		m_tokens.push_back( TextMateTokenRun( regexStyle, startOffset + lastEnd, regex->matchStart + regex->matchLength - lastEnd ) );
}

void TextMateAnnotator::Tokenize( const char* docStart, const char* docEnd )
{
	TextMateStack stack;

	const char* textStart   = docStart;
	const char* searchStart = docStart;
	const char* searchEnd   = std::find( searchStart, docEnd, 0x0A );

	while ( searchStart < docEnd )
	{
		if ( stack.IsEmpty() )
		{
			TextMateBestMatch patternMatch = FindBestRegexMatch( textStart, searchEnd, searchStart, m_language.defaultPatterns->begins );

			if ( patternMatch.index >= 0 )
			{
				TextMatePatterns* patterns = m_language.defaultPatterns;
				int32_t           best     = patternMatch.index;

				AddTokens( textStart - docStart, m_defaultStyle, patterns->styles[best], patterns->begins[best] );

				searchStart = textStart + patternMatch.start + patternMatch.length;

				if ( patterns->ends[best] )
				{
					stack.Push( patterns->styles[best], patterns->ends[best], patterns->patterns[best] );

					textStart = searchStart;
					searchEnd = docEnd;
				}
			}
			else
			{
				searchStart = (std::min)( searchEnd + 1, docEnd );
				textStart   = searchStart;
				searchEnd   = std::find( searchStart, docEnd, 0x0A );
			}
		}
		else
		{
			TextMateBestMatch patternMatch = FindBestRegexMatch( textStart, searchEnd, searchStart, stack.patternsList.back()->begins );
			TextMateBestMatch endMatch     = FindBestRegexMatch( textStart, searchEnd, searchStart, stack.ends );

			if ( endMatch.IsBetterThan( patternMatch.start, patternMatch.length ) )
			{
				AddTokens( textStart - docStart, stack.styles.back(), stack.styles[endMatch.index], stack.ends[endMatch.index] );

				searchStart = textStart + endMatch.start + endMatch.length;

				stack.PopTo( endMatch.index );

				if ( stack.IsEmpty() )
				{
					textStart = searchStart;
					searchEnd = std::find( searchStart, docEnd, 0x0A );
				}
			}
			else if ( patternMatch.index >= 0 )
			{
				TextMatePatterns* patterns = stack.patternsList.back();
				int32_t           best     = patternMatch.index;

				AddTokens( textStart - docStart, stack.styles.back(), patterns->styles[best], patterns->begins[best] );

				searchStart = textStart + patternMatch.start + patternMatch.length;

				if ( patterns->ends[best] )
					stack.Push( patterns->styles[best], patterns->ends[best], patterns->patterns[best] );
			}
			else
			{
				return;
			}
		}
	}
}

void TextMateAnnotator::TextChanged( TextChange )
{
	m_tokens.clear();

	TextDocumentReader reader( m_doc );

	AsciiRef text = reader.AsciiRange( 0, m_doc.Length() );

	if ( m_language.defaultPatterns )
		Tokenize( text.begin(), text.end() );
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
