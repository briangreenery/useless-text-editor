// TextStyleRun.h

#ifndef TextStyleRun_h
#define TextStyleRun_h

#include <stdint.h>
#include <vector>

struct TextStyleRun
{
	TextStyleRun() {}
	TextStyleRun( uint32_t styleid, size_t start, size_t count );

	uint32_t styleid;
	size_t start;
	size_t count;
};

typedef std::vector<TextStyleRun> TextStyleRuns;

#endif
