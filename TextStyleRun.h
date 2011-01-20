// TextStyleRun.h

#ifndef TextStyleRun_h
#define TextStyleRun_h

#include "Integers.h"
#include <vector>

struct TextStyleRun
{
	TextStyleRun() {}
	TextStyleRun( uint32 styleid, size_t start, size_t count );

	uint32 styleid;
	size_t start;
	size_t count;
};

typedef std::vector<TextStyleRun> TextStyleRuns;

#endif
