// TextRange.h

#ifndef TextRange_h
#define TextRange_h

#include <vector>

class TextRange
{
public:
	TextRange();
	TextRange( size_t start, size_t count );

	size_t start;
	size_t count;
};

typedef std::vector<TextRange> TextRanges;

#endif
