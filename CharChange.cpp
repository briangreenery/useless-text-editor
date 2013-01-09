// CharChange.cpp

#include "CharChange.h"
#include <algorithm>

CharChange::CharChange()
	: start( 0 )
	, end( 0 )
	, delta( 0 )
{
}

CharChange::CharChange( size_t pos, size_t count, Type type )
	: start( pos )
	, end( pos )
	, delta( 0 )
{
	if ( type == insertion )
	{
		delta = static_cast<int32_t>( count );
	}
	else
	{
		end = start + count;
		delta = -static_cast<int32_t>( count );
	}
}

void CharChange::operator+=( CharChange change )
{
	if ( start == end && delta == 0 )
	{
		start = change.start;
		end = change.end;
	}
	else
	{
		if ( change.start > start )
			change.start -= std::min<int32_t>( change.start - start, delta );

		if ( change.end > start )
			change.end -= std::min<int32_t>( change.end - start, delta );

		start = std::min( start, change.start );
		end = std::max( end, change.end );
	}

	delta += change.delta;
}
