// TextMatePattern.cpp

#include "TextMatePattern.h"
#include <algorithm>

TextMateCapture::TextMateCapture( uint32_t index, const std::string& name )
	: index( index )
	, name( name )
{
}

struct SortCaptureByIndex
{
	bool operator()( const TextMateCapture& a, const TextMateCapture& b ) const
	{
		return a.index < b.index;
	}
};

TextMatePattern::TextMatePattern( const std::string& name, const std::string& match, const std::vector<TextMateCapture>& captures )
	: name( name )
	, regex( match, captures.empty() ? std::regex::ECMAScript|std::regex::nosubs : std::regex::ECMAScript )
	, captures( captures )
{
	std::sort( this->captures.begin(), this->captures.end(), SortCaptureByIndex() );
}
