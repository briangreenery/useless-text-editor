// TextChange.h

#ifndef TextChange_h
#define TextChange_h

#include <cstddef>

class TextChange
{
public:
	static TextChange NoChange();
	static TextChange Insertion( size_t pos, size_t count );
	static TextChange Deletion ( size_t pos, size_t count );

	enum Type { noChange, insertion, deletion };

	size_t pos;
	size_t count;
	Type   type;
};

#endif
