// CharSelection.h

#ifndef CharSelection_h
#define CharSelection_h

// CharSelection is a range of characters where the end can be less than the start.

class CharSelection
{
public:
	CharSelection();
	CharSelection( size_t start, size_t end );

	size_t Min() const;
	size_t Max() const;
	size_t Length() const;

	size_t start;
	size_t end;
};

#endif
