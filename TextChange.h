// TextChange.h

#ifndef TextChange_h
#define TextChange_h

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
