// TextMatePattern.h

#ifndef TextMatePattern_h
#define TextMatePattern_h

#include <string>
#include <regex>
#include <vector>
#include <stdint.h>

class TextStyleRegistry;

class TextMateCapture
{
public:
	TextMateCapture( uint32_t index, uint32_t classID );

	uint32_t index;
	uint32_t classID;
};

class TextMatePattern
{
public:
	TextMatePattern( uint32_t classID, const std::string& match, const std::vector<TextMateCapture>& captures );

	uint32_t classID;
	std::vector<TextMateCapture> captures;

	std::regex regex;
	std::cmatch match;
};

std::vector<TextMatePattern> ReadTextMateFile( const char* path, TextStyleRegistry& styleRegistry );

#endif
