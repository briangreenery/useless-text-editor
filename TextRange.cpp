// TextRange.cpp

#include "TextRange.h"

TextRange::TextRange()
	: start( 0 )
	, count( 0 )
{
}

TextRange::TextRange( size_t _start, size_t _count )
	: start( _start )
	, count( _count )
{
}
