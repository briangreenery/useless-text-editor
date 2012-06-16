// TextChangeTest.cpp

#include "TextChangeTest.h"
#include "TextChange.h"
#include <cassert>

static void SimpleTests()
{
	{
		TextChange change;
		assert( change.start == 0 );
		assert( change.end == 0 );
		assert( change.delta == 0 );
		change.AddChange( TextChange() );
		assert( change.start == 0 );
		assert( change.end == 0 );
		assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 5, TextChange::insertion ) );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 5 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 5, TextChange::deletion ) );
		assert( change.start == 1 );
		assert( change.end == 6 );
		assert( change.delta == -5 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 5, TextChange::modification ) );
		assert( change.start == 1 );
		assert( change.end == 6 );
		assert( change.delta == 0 );
	}
}

static void ComplicatedTests()
{
	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 6 );
	}
	
	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::deletion ) );
		change.AddChange( TextChange( 1, 3, TextChange::deletion ) );
		assert( change.start == 1 );
		assert( change.end == 7 );
		assert( change.delta == -6 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::modification ) );
		change.AddChange( TextChange( 1, 3, TextChange::modification  ) );
		assert( change.start == 1 );
		assert( change.end == 4 );
		assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::modification ) );
		change.AddChange( TextChange( 0, 10, TextChange::modification ) );
		assert( change.start == 0 );
		assert( change.end == 10 );
		assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 1, 3, TextChange::deletion ) );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 9, 3, TextChange::insertion ) );
		assert( change.start == 1 );
		assert( change.end == 6 );
		assert( change.delta == 6 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 9, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		assert( change.start == 1 );
		assert( change.end == 9 );
		assert( change.delta == 6 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 9, TextChange::insertion ) );
		change.AddChange( TextChange( 2, 1, TextChange::insertion ) );
		assert( change.start == 1 );
		assert( change.end == 1 );
		assert( change.delta == 10 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 9, TextChange::deletion ) );
		change.AddChange( TextChange( 2, 1, TextChange::deletion ) );
		assert( change.start == 1 );
		assert( change.end == 12 );
		assert( change.delta == -10 );
	}
}

void TextChangeTest()
{
	SimpleTests();
	ComplicatedTests();
}
