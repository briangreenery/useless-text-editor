// SimpleLayoutData.cpp

#include "SimpleLayoutData.h"
#include "Assert.h"
#include <numeric>

SimpleLayoutData::SimpleLayoutData( UTF16Ref text, bool _endsWithNewline )
	: length( text.size() )
	, endsWithNewline( _endsWithNewline )
{
	if ( endsWithNewline )
		length++;
}

void SimpleLayoutData::DiscardFrom( size_t position )
{
	Assert( !runs.empty() );
	Assert( position <= runs.back().textStart + runs.back().textCount );
	Assert( runs.front().textStart == 0 );

	size_t i = runs.size() - 1;
	while ( runs[i].textStart > position )
		--i;

	if ( runs[i].textStart < position )
	{
		runs[i].textCount = position - runs[i].textStart;
		runs.erase( runs.begin() + i + 1, runs.end() );
	}
	else
	{
		runs.erase( runs.begin() + i, runs.end() );
	}

	xOffsets.erase( xOffsets.begin() + position, xOffsets.end() );
}
