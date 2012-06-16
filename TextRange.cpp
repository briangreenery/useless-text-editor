// TextRange.cpp

#include "TextRange.h"

TextRange::TextRange()
	: start( 0 )
	, count( 0 )
{
}

TextRange::TextRange( size_t start, size_t count )
	: start( start )
	, count( count )
{
}
