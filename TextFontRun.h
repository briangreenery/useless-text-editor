// TextFontRun.h

#ifndef TextFontRun_h
#define TextFontRun_h

#include <stdint.h>
#include <vector>

class TextFontRun
{
public:
	TextFontRun() {}
	TextFontRun( uint32_t fontid, size_t count );

	uint32_t fontid;
	size_t count;
};

typedef std::vector<TextFontRun> TextFontRuns;

#endif
