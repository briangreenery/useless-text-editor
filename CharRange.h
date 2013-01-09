// CharRange.h

#ifndef CharRange_h
#define CharRange_h

// CharRange is a range of characters.

class CharRange
{
public:
	CharRange();
	CharRange( size_t start, size_t count );

	size_t start;
	size_t count;
};

#endif
