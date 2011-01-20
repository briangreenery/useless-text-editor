// UniscribeTextRun.h

#ifndef UniscribeTextRun_h
#define UniscribeTextRun_h

#include "Integers.h"

class UniscribeTextRun
{
public:
	UniscribeTextRun() {}
	UniscribeTextRun( size_t item, size_t textStart, size_t textCount, uint32 fontid );

	size_t item;

	size_t glyphStart;
	size_t glyphCount;

	size_t textStart;
	size_t textCount;
	uint32 fontid;

	int width;
};

#endif
