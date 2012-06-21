// TextMatePattern.cpp

#include "TextMatePattern.h"
#include "TextStyleRegistry.h"
#include "rapidxml\rapidxml.hpp"
#include <algorithm>
#include <streambuf>
#include <fstream>

using namespace rapidxml;

TextMateCapture::TextMateCapture( uint32_t index, uint32_t classID )
	: index( index )
	, classID( classID )
{
}

struct SortCaptureByIndex
{
	bool operator()( const TextMateCapture& a, const TextMateCapture& b ) const
	{
		return a.index < b.index;
	}
};

TextMatePattern::TextMatePattern( uint32_t classID, const std::string& match, const std::vector<TextMateCapture>& captures )
	: classID( classID )
	, regex( match, captures.empty() ? std::regex::ECMAScript|std::regex::nosubs : std::regex::ECMAScript )
	, captures( captures )
{
	std::sort( this->captures.begin(), this->captures.end(), SortCaptureByIndex() );
}

static xml_node<char>* GetKeyValue( xml_node<char>* node, const char* keyName )
{
	for ( xml_node<char>* key = node->first_node( "key" ); key != 0; key = key = key->next_sibling( "key" ) )
		if ( strcmp( key->value(), keyName ) == 0 )
			return key->next_sibling();

	return 0;
}

static std::vector<TextMateCapture> ReadCaptures( xml_node<char>* node, TextStyleRegistry& styleRegistry )
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

		captures.push_back( TextMateCapture( index, styleRegistry.ClassID( name->value() ) ) );
	}

	return captures;
}

std::vector<TextMatePattern> ReadTextMateFile( const char* path, TextStyleRegistry& styleRegistry )
{
	std::ifstream t( path );
	std::string xml( ( std::istreambuf_iterator<char>( t ) ), std::istreambuf_iterator<char>() );

	std::vector<TextMatePattern> patterns;

	xml_document<char> doc;
	doc.parse<parse_default>( const_cast<char*>( xml.c_str() ) );

	xml_node<char>* plistNode = doc.first_node( "plist" );
	if ( plistNode == 0 )
		return patterns;

	xml_node<char>* dictNode = plistNode->first_node( "dict" );
	if ( dictNode == 0 )
		return patterns;

	xml_node<char>* patternsNode = GetKeyValue( dictNode, "patterns" );
	if ( patternsNode == 0 )
		return patterns;

	for ( xml_node<char>* dictNode = patternsNode->first_node( "dict" ); dictNode != 0; dictNode = dictNode->next_sibling( "dict" ) )
	{
		if ( GetKeyValue( dictNode, "begin" ) || GetKeyValue( dictNode, "end" ) )
			continue;

		xml_node<char>* nameNode  = GetKeyValue( dictNode, "name" );
		xml_node<char>* matchNode = GetKeyValue( dictNode, "match" );

		if ( nameNode == 0 || matchNode == 0 )
			continue;

		std::vector<TextMateCapture> captures;

		xml_node<char>* capturesNode = GetKeyValue( dictNode, "captures" );

		if ( capturesNode )
			captures = ReadCaptures( capturesNode, styleRegistry );

		try
		{
			patterns.push_back( TextMatePattern( styleRegistry.ClassID( nameNode->value() ), matchNode->value(), captures ) );
		}
		catch (...)
		{
		}
	}

	return patterns;
}
