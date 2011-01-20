// TextFontRun.h

#ifndef TextFontRun_h
#define TextFontRun_h

#include "Integers.h"
#include <vector>

class TextFontRun
{
public:
	TextFontRun() {}
	TextFontRun( uint32 fontid, size_t count );

	uint32 fontid;
	size_t count;
};

typedef std::vector<TextFontRun> TextFontRuns;

#endif
