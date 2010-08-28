// UniscribeTextRun.h

#ifndef UniscribeTextRun_h
#define UniscribeTextRun_h

struct UniscribeTextRun
{
	size_t item;

	size_t glyphStart;
	size_t glyphCount;

	size_t textStart;
	size_t textCount;

	int width;

	size_t font;
};

#endif
