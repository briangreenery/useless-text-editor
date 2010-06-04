// TextRun.h

#ifndef TextRun_h
#define TextRun_h

#include "Integers.h"

struct TextRun
{
	size_t item;

	size_t glyphStart;
	size_t glyphCount;

	size_t textStart;
	size_t textCount;

	int width;
	int style;
};

#endif
