// TextMatePattern.cpp

#include "TextMatePattern.h"
#include <algorithm>

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
