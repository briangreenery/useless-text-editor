// TextMatePattern.cpp

#include "TextMatePattern.h"

TextMatePattern::TextMatePattern( const std::string& name, const std::string& match )
	: name( name )
	, regex( match, std::regex::ECMAScript|std::regex::nosubs )
{
}
