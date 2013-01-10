// TextMateAnnotator.cpp

#include "TextMateAnnotator.h"
#include "Document.h"
#include "TextStyleRegistry.h"
#include <string>
#include <algorithm>

TextMateAnnotator::TextMateAnnotator( const Document& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_styleRegistry( styleRegistry )
	, m_defaultStyle( styleRegistry.ClassID( "default" ) )
{
}

void TextMateAnnotator::SetLanguageFile( const char* path )
{
	m_language = ReadTextMateFile( path, m_styleRegistry );
}

static TextMateBestMatch FindBestMatch( const char* lineStart,
                                        const char* searchStart,
                                        const char* lineEnd,
                                        bool isFirstSearch,
                                        const std::vector<TextMateRegex*>& regexes )
{
	TextMateBestMatch bestMatch;

	for ( int32_t i = 0, size = regexes.size(); i < size; ++i )
	{
		TextMateRegex* regex = regexes[i];

		if ( isFirstSearch || ( regex->matchStart >= 0 && regex->matchStart < searchStart - lineStart ) )
			regex->matchStart = onig_search( regex->regex.get(),
			                                 reinterpret_cast<const OnigUChar*>( lineStart ),
			                                 reinterpret_cast<const OnigUChar*>( lineEnd ),
			                                 reinterpret_cast<const OnigUChar*>( searchStart ),
			                                 reinterpret_cast<const OnigUChar*>( lineEnd ),
			                                 regex->region.get(),
			                                 ONIG_OPTION_NONE );

		if ( regex->matchStart < 0 )
			continue;

		uint32_t matchLength = regex->region->end[0] - regex->matchStart;

		if ( !bestMatch.IsBetterThan( regex->matchStart, matchLength ) )
		{
			bestMatch.index  = i;
			bestMatch.start  = regex->matchStart;
			bestMatch.length = matchLength;
		}
	}

	return bestMatch;
}

void TextMateAnnotator::AddTokensUpTo( size_t offset, uint32_t style )
{
	size_t lastEnd = m_tokens.empty() ? 0 : m_tokens.back().start + m_tokens.back().count;

	if ( lastEnd != offset )
		m_tokens.push_back( TextMateTokenRun( style, lastEnd, offset - lastEnd ) );
}

void TextMateAnnotator::AddTokens( const TextMateRegex* regex, const TextMateCaptures& captures, uint32_t style, size_t offset )
{
	size_t lastEnd = regex->region->beg[0];

	for ( auto capture : captures )
	{
		if ( capture.index > uint32_t( regex->region->num_regs ) )
			break;

		size_t start  = regex->region->beg[capture.index];
		size_t length = regex->region->end[capture.index] - start;

		if ( length == 0 )
			continue;

		if ( lastEnd != start )
			m_tokens.push_back( TextMateTokenRun( style, offset + lastEnd, start - lastEnd ) );

		m_tokens.push_back( TextMateTokenRun( capture.style, offset + start, length ) );
		lastEnd = start + length;
	}

	if ( lastEnd != regex->region->end[0] )
		m_tokens.push_back( TextMateTokenRun( style, offset + lastEnd, regex->region->end[0] - lastEnd ) );
}

void TextMateAnnotator::Tokenize( const char* docStart, const char* docEnd )
{
	TextMatePatterns* patterns = m_language.defaultPatterns;
	std::vector<TextMatePatterns*> patternsStack;

	uint32_t style = m_defaultStyle;
	std::vector<uint32_t> styleStack;

	const char* lineStart   = docStart;
	const char* searchStart = lineStart;
	const char* lineEnd     = std::find( lineStart, docEnd, 0x0A );

	bool isFirstSearch = true;

	while ( searchStart != docEnd )
	{
		TextMateBestMatch match;
		
		if ( searchStart != lineEnd )
			match = FindBestMatch( lineStart, searchStart, lineEnd, isFirstSearch, patterns->regexes );

		if ( match.index >= 0 )
		{
			AddTokensUpTo( lineStart - docStart + match.start, style );

			AddTokens( patterns->regexes[match.index],
			           patterns->captures[match.index],
			           patterns->styles[match.index],
			           lineStart - docStart );

			searchStart = lineStart + match.start + match.length;

			if ( match.index == 0 && !patternsStack.empty() )
			{
				style = styleStack.back();
				styleStack.pop_back();

				patterns = patternsStack.back();
				patternsStack.pop_back();

				isFirstSearch = true;
			}
			else if ( patterns->patterns[match.index] )
			{
				styleStack.push_back( style );
				style = patterns->styles[match.index];

				patternsStack.push_back( patterns );
				patterns = patterns->patterns[match.index];

				isFirstSearch = true;
			}
		}
		else
		{
			lineStart   = (std::min)( lineEnd + 1, docEnd );
			searchStart = lineStart;
			lineEnd     = std::find( lineStart, docEnd, 0x0A );

			isFirstSearch = true;
		}
	}

	AddTokensUpTo( docEnd - docStart, style );
}

void TextMateAnnotator::TextChanged( CharChange )
{
	m_tokens.clear();

	ArrayRef<const wchar_t> text = m_doc.Chars().Read( 0, m_doc.Chars().Length() );

	std::vector<char> ascii;
	ascii.resize( text.size() );

	for ( size_t i = 0; i < text.size(); ++i )
		ascii[i] = (char)(std::max<wchar_t>)( text[i], 127 );

	if ( !ascii.empty() && m_language.defaultPatterns )
		Tokenize( &ascii.front(), &ascii.front() + ascii.size() );
}

void TextMateAnnotator::SelectionChanged( size_t start, size_t end )
{
}

typedef std::pair<TextMateTokenRuns::const_iterator, TextMateTokenRuns::const_iterator> TokenRange;

struct TokenRunCompare
{
	bool operator()( const TextMateTokenRun& a, const TextMateTokenRun&  b ) const { return a.start + a.count <= b.start; }
	bool operator()( const TextMateTokenRun& a, const CharRange&         b ) const { return a.start + a.count <= b.start; }
	bool operator()( const CharRange&        a, const TextMateTokenRun&  b ) const { return a.start + a.count <= b.start; }
};

void TextMateAnnotator::GetClasses( TextStyleRuns& styles, size_t start, size_t count )
{
	CharRange CharRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), CharRange, TokenRunCompare() );

	for ( TextMateTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = (std::max)( start, it->start );
		size_t overlapEnd   = (std::min)( start + count, it->start + it->count );

		styles.push_back( TextStyleRun( it->classID, overlapStart, overlapEnd - overlapStart ) );
	}
}

void TextMateAnnotator::GetSquiggles( CharRanges&, size_t start, size_t count )
{
}

void TextMateAnnotator::GetHighlights( CharRanges&, size_t start, size_t count )
{
}
