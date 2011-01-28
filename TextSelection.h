// TextSelection.h

#ifndef TextSelection_h
#define TextSelection_h

#include "Assert.h"
#include <Windows.h>
#include <vector>
#include <algorithm>

#undef min
#undef max

class TextSelection
{
public:
	TextSelection();

	bool Intersects( size_t start, size_t end ) const;

	size_t Min() const { return std::min( start, end ); }
	size_t Max() const { return std::max( start, end ); }

	bool IsEmpty() const { return start == end; }
	size_t Size() const  { return Max() - Min(); }

	size_t start;
	size_t end;
	POINT  endPoint;
};

bool operator==( const TextSelection&, const TextSelection& );
bool operator!=( const TextSelection&, const TextSelection& );

#endif
