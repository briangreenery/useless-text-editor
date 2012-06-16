// SimpleLayoutData.cpp

#include "SimpleLayoutData.h"
#include <cassert>
#include <numeric>

SimpleLayoutData::SimpleLayoutData( UTF16Ref text, bool endsWithNewline )
	: length( text.size() )
	, endsWithNewline( endsWithNewline )
{
	if ( endsWithNewline )
		length++;

	xOffsets.resize( text.size() );
}

void SimpleLayoutData::DiscardFrom( size_t position )
{
	assert( !runs.empty() );
	assert( position <= runs.back().textStart + runs.back().textCount );
	assert( runs.front().textStart == 0 );

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
}
