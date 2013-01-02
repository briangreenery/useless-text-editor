// TextMatePattern.cpp

#include "TextMatePattern.h"
#include "TextStyleRegistry.h"
#include "rapidxml.hpp"
#include <algorithm>
#include <streambuf>
#include <fstream>

using namespace rapidxml;

TextMateCapture::TextMateCapture( uint32_t index, uint32_t style )
	: index( index )
	, style( style )
{
}

TextMateBestMatch::TextMateBestMatch()
	: index( -1 )
	, start( (std::numeric_limits<int32_t>::max)() )
	, length( -1 )
{
}

bool TextMateBestMatch::IsBetterThan( int32_t otherStart, int32_t otherLength ) const
{
	return index >= 0 && ( start < otherStart || ( start == otherStart && length >= otherLength ) );
}

TextMateLanguage::TextMateLanguage()
	: defaultPatterns( 0 )
{
}

TextMateRegex* TextMateLanguage::NewRegex( const char* pattern )
{
	regex_t* regex;

	const OnigUChar* patternStart = reinterpret_cast<const uint8_t*>( pattern );
	const OnigUChar* patternEnd   = patternStart + strlen( pattern );

	int status = onig_new( &regex, patternStart, patternEnd, ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT, 0 );

	if ( status != ONIG_NORMAL )
		return 0;

	TextMateRegexPtr textMateRegex( new TextMateRegex( OnigRegexPtr( regex, onig_free ) ) );
	regexes.push_back( textMateRegex );
	return textMateRegex.get();
}

TextMatePatterns* TextMateLanguage::NewPatterns()
{
	TextMatePatternsPtr textMatePatterns( new TextMatePatterns );
	patterns.push_back( textMatePatterns );
	return textMatePatterns.get();
}

static void FreeOnigRegion( OnigRegion* region )
{
	onig_region_free( region, 1 );
}

TextMateRegex::TextMateRegex( OnigRegexPtr regex )
	: regex( regex )
	, region( onig_region_new(), FreeOnigRegion )
	, matchStart( -1 )
{
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

	std::sort( captures.begin(), captures.end(), []( const TextMateCapture& a, const TextMateCapture& b ) { return a.index < b.index; } );
	return captures;
}

static void ReadTextMatePatterns( TextMateLanguage& language, TextMatePatterns* patterns, xml_node<char>* patternsNode, TextStyleRegistry& styleRegistry )
{
	for ( xml_node<char>* dictNode = patternsNode->first_node( "dict" ); dictNode != 0; dictNode = dictNode->next_sibling( "dict" ) )
	{
		xml_node<char>* beginNode         = GetKeyValue( dictNode, "begin" );
		xml_node<char>* beginCapturesNode = GetKeyValue( dictNode, "beginCaptures" );
		xml_node<char>* endNode           = GetKeyValue( dictNode, "end" );
		xml_node<char>* endCapturesNode   = GetKeyValue( dictNode, "endCaptures" );
		xml_node<char>* nameNode          = GetKeyValue( dictNode, "name" );
		xml_node<char>* matchNode         = GetKeyValue( dictNode, "match" );
		xml_node<char>* capturesNode      = GetKeyValue( dictNode, "captures" );
		xml_node<char>* subPatternsNode   = GetKeyValue( dictNode, "patterns" );

		if ( nameNode == 0 )
			continue;

		std::vector<TextMateCapture> captures;
		if ( capturesNode )
			captures = ReadCaptures( capturesNode, styleRegistry );

		if ( beginNode && endNode )
		{
			std::vector<TextMateCapture> beginCaptures = beginCapturesNode ? ReadCaptures( beginCapturesNode, styleRegistry ) : captures;
			std::vector<TextMateCapture> endCaptures   = endCapturesNode ? ReadCaptures( endCapturesNode, styleRegistry ) : captures;

			TextMateRegex* begin = language.NewRegex( beginNode->value() );
			if ( begin == 0 )
				continue;

			TextMateRegex* end = language.NewRegex( endNode->value() );
			if ( end == 0 )
				continue;

			TextMatePatterns* subPatterns = language.NewPatterns();

			subPatterns->regexes.push_back( end );
			subPatterns->captures.push_back( endCaptures );
			subPatterns->patterns.push_back( 0 );
			subPatterns->styles.push_back( styleRegistry.ClassID( nameNode->value() ) );

			if ( subPatternsNode )
				ReadTextMatePatterns( language, subPatterns, subPatternsNode, styleRegistry );

			patterns->regexes.push_back( begin );
			patterns->captures.push_back( beginCaptures );
			patterns->patterns.push_back( subPatterns );
			patterns->styles.push_back( styleRegistry.ClassID( nameNode->value() ) );
		}
		else if ( matchNode )
		{
			TextMateRegex* match = language.NewRegex( matchNode->value() );
			if ( match == 0 )
				continue;

			patterns->regexes.push_back( match );
			patterns->captures.push_back( captures );
			patterns->patterns.push_back( 0 );
			patterns->styles.push_back( styleRegistry.ClassID( nameNode->value() ) );
		}
	}
}

TextMateLanguage ReadTextMateFile( const char* path, TextStyleRegistry& styleRegistry )
{
	std::ifstream t( path );
	std::string xml( ( std::istreambuf_iterator<char>( t ) ), std::istreambuf_iterator<char>() );

	TextMateLanguage language;

	xml_document<char> doc;
	doc.parse<parse_default>( const_cast<char*>( xml.c_str() ) );

	xml_node<char>* plistNode = doc.first_node( "plist" );
	if ( plistNode == 0 )
		return language;

	xml_node<char>* dictNode = plistNode->first_node( "dict" );
	if ( dictNode == 0 )
		return language;

	xml_node<char>* patternsNode = GetKeyValue( dictNode, "patterns" );
	if ( patternsNode == 0 )
		return language;

	language.defaultPatterns = language.NewPatterns();
	ReadTextMatePatterns( language, language.defaultPatterns, patternsNode, styleRegistry );
	return language;
}
