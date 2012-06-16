// UniscribeTextRun.h

#ifndef UniscribeTextRun_h
#define UniscribeTextRun_h

#include <stdint.h>

class UniscribeTextRun
{
public:
	UniscribeTextRun() {}
	UniscribeTextRun( size_t item, size_t textStart, size_t textCount, uint32_t fontid );

	size_t item;

	size_t glyphStart;
	size_t glyphCount;

	size_t textStart;
	size_t textCount;
	uint32_t fontid;

	int width;
};

#endif
