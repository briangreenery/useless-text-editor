// TextMateAnnotator.cpp

#include "TextMateAnnotator.h"
#include "rapidxml\rapidxml.hpp"
#include "TextDocumentReader.h"
#include "TextDocument.h"
#include "TextStyleRegistry.h"
#include <streambuf>
#include <fstream>
#include <string>

using namespace rapidxml;

TextMateAnnotator::TextMateAnnotator( const TextDocument& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_styleRegistry( styleRegistry )
{
	m_keyword  = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 0,    0,255 ), TextStyle::useDefault ) );
	m_constant = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 128,  0,128 ), TextStyle::useDefault ) );
	m_string   = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 0,  128,128 ), TextStyle::useDefault ) );
	m_name     = styleRegistry.AddStyle( TextStyle( TextStyle::useDefault, RGB( 128,  0,128 ), TextStyle::useDefault ) );
}

static xml_node<char>* GetKeyValue( xml_node<char>* node, const char* keyName )
{
	for ( xml_node<char>* key = node->first_node( "key" ); key != 0; key = key = key->next_sibling( "key" ) )
		if ( strcmp( key->value(), keyName ) == 0 )
			return key->next_sibling();

	return 0;
}

static std::vector<TextMateCapture> ReadCaptures( xml_node<char>* node )
{
	std::vector<TextMateCapture> captures;

	for ( xml_node<char>* key = node->first_node( "key" ); key != 0; key = key->next_sibling( "key" ) )
	{
		int index = atoi( key->value() );

		if ( index <= 0 )
			continue;

		xml_node<char>* dict = key->next_sibling();
		if ( dict == 0 )
			continue;

		xml_node<char>* name = GetKeyValue( dict, "name" );
		if ( name == 0 )
			continue;

		captures.push_back( TextMateCapture( index, name->value() ) );
	}

	return captures;
}

void TextMateAnnotator::SetLanguageFile( const char* path )
{
	std::ifstream t( path );
	std::string xml( ( std::istreambuf_iterator<char>( t ) ), std::istreambuf_iterator<char>() );

	xml_document<char> doc;
	doc.parse<parse_default>( const_cast<char*>( xml.c_str() ) );

	xml_node<char>* plist = doc.first_node( "plist" );
	if ( plist == 0 )
		return;

	xml_node<char>* dict = plist->first_node( "dict" );
	if ( dict == 0 )
		return;

	xml_node<char>* patterns = GetKeyValue( dict, "patterns" );
	if ( patterns == 0 )
		return;

	for ( xml_node<char>* dict = patterns->first_node( "dict" ); dict != 0; dict = dict->next_sibling( "dict" ) )
	{
		if ( GetKeyValue( dict, "begin" ) || GetKeyValue( dict, "end" ) )
			continue;

		xml_node<char>* nameNode  = GetKeyValue( dict, "name" );
		xml_node<char>* matchNode = GetKeyValue( dict, "match" );

		if ( nameNode == 0 || matchNode == 0 )
			continue;

		std::vector<TextMateCapture> captures;

		xml_node<char>* capturesNode = GetKeyValue( dict, "captures" );

		if ( capturesNode )
			captures = ReadCaptures( capturesNode );

		try
		{
			m_patterns.push_back( TextMatePattern( nameNode->value(), matchNode->value(), captures ) );
		}
		catch (...)
		{
		}
	}
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
			if ( bestStart != offset )
				m_tokens.push_back( TextMateTokenRun( "default", offset, bestStart - offset ) );

			m_tokens.push_back( TextMateTokenRun( m_patterns[best].name, bestStart, bestLength ) );
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

				if ( start != lastEnd )
					m_tokens.push_back( TextMateTokenRun( "default", lastEnd, start - lastEnd ) );

				m_tokens.push_back( TextMateTokenRun( pattern.captures[i].name, start, length ) );
				lastEnd = start + length;
			}

			if ( lastEnd != bestStart + bestLength )
				m_tokens.push_back( TextMateTokenRun( "default", lastEnd, bestStart + bestLength - lastEnd ) );
		}

		line = line.substr( nextStart );
		offset = bestStart + bestLength;
	}

	if ( offset != lineEnd )
		m_tokens.push_back( TextMateTokenRun( "default", offset, lineEnd - offset ) );
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

		m_tokens.push_back( TextMateTokenRun( "default", offset + line.size(), 1 ) );
		offset += line.size() + 1;
	}
}

void TextMateAnnotator::SelectionChanged( size_t start, size_t end )
{
}

void TextMateAnnotator::GetFonts( TextFontRuns& runs, size_t start, size_t count )
{
}

typedef std::pair<TextMateTokenRuns::const_iterator, TextMateTokenRuns::const_iterator> TokenRange;

struct TokenRunCompare
{
	bool operator()( const TextMateTokenRun& a, const TextMateTokenRun&  b ) const { return a.start + a.count <= b.start; }
	bool operator()( const TextMateTokenRun& a, const TextRange&         b ) const { return a.start + a.count <= b.start; }
	bool operator()( const TextRange&        a, const TextMateTokenRun&  b ) const { return a.start + a.count <= b.start; }
};

uint32_t TextMateAnnotator::TokenStyle( size_t pos ) const
{
	const TextMateTokenRun& run = m_tokens[pos];

	if ( run.name == "default" )
		return m_styleRegistry.defaultStyleid;

	if ( run.name == "storage.type.js" )
		return m_keyword;

	if ( run.name.find( "keyword." ) == 0 )
		return m_keyword;

	if ( run.name.find( "constant." ) == 0 )
		return m_constant;

	if ( run.name.find( "entity.name." ) == 0 )
		return m_name;

	return m_styleRegistry.defaultStyleid;
}

void TextMateAnnotator::GetStyles( TextStyleRuns& styles, size_t start, size_t count )
{
	TextRange textRange( start, count );
	TokenRange range = std::equal_range( m_tokens.begin(), m_tokens.end(), textRange, TokenRunCompare() );

	for ( TextMateTokenRuns::const_iterator it = range.first; it != range.second; ++it )
	{
		size_t overlapStart = (std::max)( start, it->start );
		size_t overlapEnd   = (std::min)( start + count, it->start + it->count );

		uint32_t styleid = TokenStyle( it - m_tokens.begin() );

		if ( !styles.empty() && styles.back().styleid == styleid )
			styles.back().count += overlapEnd - overlapStart;
		else
			styles.push_back( TextStyleRun( styleid, overlapStart, overlapEnd - overlapStart ) );
	}
}

void TextMateAnnotator::GetSquiggles( TextRanges&, size_t start, size_t count )
{
}

void TextMateAnnotator::GetHighlights( TextRanges&, size_t start, size_t count )
{
}
