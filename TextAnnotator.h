// TextAnnotator.h

#ifndef TextAnnotator_h
#define TextAnnotator_h

#include "TextChange.h"

class TextAnnotator
{
public:
	virtual TextChange Update( TextChange ) = 0;

	virtual size_t ReadStyles( size_t* styles, size_t length ) = 0;
};

#endif
