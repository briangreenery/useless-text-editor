// TextMatePattern.h

#ifndef TextMatePattern_h
#define TextMatePattern_h

#include <string>
#include <regex>
#include <vector>
#include <stdint.h>

class TextMateCapture
{
public:
	TextMateCapture( uint32_t index, const std::string& name );

	uint32_t index;
	std::string name;
};

class TextMatePattern
{
public:
	TextMatePattern( const std::string& name, const std::string& match, const std::vector<TextMateCapture>& captures );

	std::string name;
	std::vector<TextMateCapture> captures;

	std::regex regex;
	std::cmatch match;
};

#endif
