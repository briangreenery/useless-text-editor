// TextMatePattern.h

#ifndef TextMatePattern_h
#define TextMatePattern_h

#include <string>
#include <regex>

class TextMatePattern
{
public:
	TextMatePattern( const std::string& name, const std::string& match );

	std::string name;
	std::regex regex;
};

#endif
