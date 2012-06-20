// TextMateTokenRun.h

#ifndef TextMateTokenRun_h
#define TextMateTokenRun_h

#include <string>
#include <vector>
#include <stdint.h>

class TextMateTokenRun
{
public:
	TextMateTokenRun( uint32_t classID, size_t start, size_t count );

	uint32_t classID;
	size_t   start;
	size_t   count;
};

typedef std::vector<TextMateTokenRun> TextMateTokenRuns;

#endif
