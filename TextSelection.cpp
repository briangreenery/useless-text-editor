// TextSelection.cpp

#include "TextSelection.h"

TextSelection::TextSelection()
	: start( 0 )
	, end( 0 )
{
	endPoint.x = endPoint.y = 0;
}

bool TextSelection::Intersects( size_t theStart, size_t theEnd ) const
{
	return Min() < theEnd && theStart < Max();
}
