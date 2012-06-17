// TextMateTokenRun.h

#ifndef TextMateTokenRun_h
#define TextMateTokenRun_h

#include <string>
#include <vector>

class TextMateTokenRun
{
public:
	TextMateTokenRun( const std::string& name, size_t start, size_t count );

	std::string name;
	size_t start;
	size_t count;
};

typedef std::vector<TextMateTokenRun> TextMateTokenRuns;

#endif
