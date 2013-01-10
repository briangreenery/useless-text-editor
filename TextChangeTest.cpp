// TextChangeTest.cpp

#include "TextChangeTest.h"
#include "CharChange.h"
#include <cassert>

static void SimpleTests()
{
	{
		CharChange change;
		assert( change.start == 0 );
		assert( change.end == 0 );
		assert( change.delta == 0 );
		change += CharChange();
		assert( change.start == 0 );
		assert( change.end == 0 );
		assert( change.delta == 0 );
	}

	{
		CharChange change;
		change += CharChange( 1, 5, CharChange::insertion );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 5 );
	}

	{
		CharChange change;
		change += CharChange( 1, 5, CharChange::deletion );
		assert( change.start == 1 );
		assert( change.end == 6 );
		assert( change.delta == -5 );
	}
}

static void ComplicatedTests()
{
	{
		CharChange change;
		change += CharChange( 1, 3, CharChange::insertion );
		change += CharChange( 1, 3, CharChange::insertion );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 6 );
	}
	
	{
		CharChange change;
		change += CharChange( 1, 3, CharChange::deletion );
		change += CharChange( 1, 3, CharChange::deletion );
		assert( change.start == 1 );
		assert( change.end == 7 );
		assert( change.delta == -6 );
	}

	{
		CharChange change;
		change += CharChange( 1, 3, CharChange::insertion );
		change += CharChange( 1, 3, CharChange::deletion );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 0 );
	}

	{
		CharChange change;
		change += CharChange( 1, 3, CharChange::insertion );
		change += CharChange( 9, 3, CharChange::insertion );
		assert( change.start == 1 );
		assert( change.end == 6 );
		assert( change.delta == 6 );
	}

	{
		CharChange change;
		change += CharChange( 9, 3, CharChange::insertion );
		change += CharChange( 1, 3, CharChange::insertion );
		assert( change.start == 1 );
		assert( change.end == 9 );
		assert( change.delta == 6 );
	}

	{
		CharChange change;
		change += CharChange( 1, 9, CharChange::insertion );
		change += CharChange( 2, 1, CharChange::insertion );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 10 );
	}

	{
		CharChange change;
		change += CharChange( 1, 9, CharChange::deletion );
		change += CharChange( 2, 1, CharChange::deletion );
		assert( change.start == 1 );
		assert( change.end == 12 );
		assert( change.delta == -10 );
	}
}

void CharChangeTest()
{
	SimpleTests();
	ComplicatedTests();
}
