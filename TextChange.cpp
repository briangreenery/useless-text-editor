// TextChange.cpp

#include "TextChange.h"
#include <cassert>
#include <algorithm>

TextChange::TextChange()
	: start( 0 )
	, end( 0 )
	, delta( 0 )
{
}

TextChange::TextChange( size_t pos, size_t count, Type type )
	: start( pos )
	, end( pos )
	, delta( 0 )
{
	if ( type == insertion )
	{
		delta = static_cast<int>( count );
	}
	else if ( type == deletion )
	{
		end = start + count;
		delta = -static_cast<int>( count );
	}
	else
	{
		end = start + count;
	}
}

void TextChange::AddChange( TextChange change )
{
	if ( start == end && delta == 0 )
	{
		start = change.start;
		end = change.end;
	}
	else
	{
		if ( change.start > start )
			change.start -= std::min<int>( change.start - start, delta );
		if ( change.end > start )
			change.end -= std::min<int>( change.end - start, delta );

		start = std::min( start, change.start );
		end = std::max( end, change.end );
	}

	delta += change.delta;
}
