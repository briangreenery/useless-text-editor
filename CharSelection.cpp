// CharSelection.cpp

#include "CharSelection.h"
#include <algorithm>

CharSelection::CharSelection()
	: start( 0 )
	, end( 0 )
{
}

CharSelection::CharSelection( size_t start, size_t end )
	: start( start )
	, end( end )
{
}

bool CharSelection::Intersects( size_t otherStart, size_t otheEnd ) const
{
	return Min() < otheEnd && otherStart < Max();
}

size_t CharSelection::Min() const
{
	return std::min( start, end );
}

size_t CharSelection::Max() const
{
	return std::max( start, end );
}

size_t CharSelection::Length() const
{
	return Max() - Min();
}
