// CharRange.cpp

#include "CharRange.h"

CharRange::CharRange()
	: start( 0 )
	, count( 0 )
{
}

CharRange::CharRange( size_t start, size_t count )
	: start( start )
	, count( count )
{
}
