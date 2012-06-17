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
}

static xml_node<char>* GetKeyValue( xml_node<char>* node, const char* keyName )
{
	xml_node<char>* key = node->first_node( "key" );

	while ( key != 0 )
	{
		if ( strcmp( key->value(), keyName ) == 0 )
			return key->next_sibling();

		key = key->next_sibling( "key" );
	}

	return 0;
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
		xml_node<char>* name = GetKeyValue( dict, "name" );
		xml_node<char>* match = GetKeyValue( dict, "match" );

		if ( GetKeyValue( dict, "begin" ) || GetKeyValue( dict, "end" ) || GetKeyValue( dict, "captures" ) )
			continue;

		if ( name && match )
		{
			try
			{
				m_patterns.push_back( TextMatePattern( name->value(), match->value() ) );
			}
			catch (...)
			{
			}
		}
	}

	int x = 1;
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
			std::cmatch m;

			if ( std::regex_search( line.c_str(), line.c_str() + line.size(), m, m_patterns[i].regex ) )
			{
				size_t start = m.position();
				if ( start > bestStart )
					continue;

				size_t length = m.length();
				if ( start == bestStart && length < bestLength )
					continue;

				best = i;
				bestStart = start;
				bestLength = length;
			}
		}

		if ( best == m_patterns.size() )
			break;

		line = line.substr( bestStart + bestLength );
		bestStart += offset;

		if ( bestStart != offset )
			m_tokens.push_back( TextMateTokenRun( "default", offset, bestStart - offset ) );

		m_tokens.push_back( TextMateTokenRun( m_patterns[best].name, bestStart, bestLength ) );
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
