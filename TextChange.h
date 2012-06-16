// TextChange.h

#ifndef TextChange_h
#define TextChange_h

// TextChange collects a group of changes, and calculates the start, end, and delta
// of those changes in the **original document before any changes were applied**.

class TextChange
{
public:
	enum Type { noChange, insertion, deletion, modification };

	TextChange();
	TextChange( size_t pos, size_t count, Type );

	void AddChange( TextChange );

	size_t start;
	size_t end;
	int    delta;
};

#endif
