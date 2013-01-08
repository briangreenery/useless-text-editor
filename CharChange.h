// CharChange.h

#ifndef CharChange_h
#define CharChange_h

#include <stdint.h>

// CharChange collects a group of changes, and calculates the start, end, and delta
// of those changes in the **original document before the changes were applied**.

class CharChange
{
public:
	enum Type { noChange, insertion, deletion, modification };

	CharChange();
	CharChange( size_t pos, size_t count, Type );

	void operator+=( CharChange );

	size_t  start;
	size_t  end;
	int32_t delta;
};

#endif
