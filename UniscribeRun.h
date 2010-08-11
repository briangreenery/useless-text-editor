// UniscribeRun.h

#ifndef UniscribeRun_h
#define UniscribeRun_h

#include "Integers.h"

struct UniscribeRun
{
	size_t item;

	size_t glyphStart;
	size_t glyphCount;

	size_t textStart;
	size_t textCount;

	int width;
	int font;
};

#endif
