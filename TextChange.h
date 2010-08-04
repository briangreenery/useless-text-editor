// TextChange.h

#ifndef TextChange_h
#define TextChange_h

#include <vector>

class TextChange
{
public:
	static TextChange NoChange();
	static TextChange Insertion   ( size_t pos, size_t count );
	static TextChange Deletion    ( size_t pos, size_t count );
	static TextChange Modification( size_t pos, size_t count );

	enum Type { noChange, insertion, deletion, modification };

	size_t pos;
	size_t count;
	Type   type;
};

typedef std::vector<TextChange> TextChanges;

#endif
