// TextChangeTest.cpp

#include "TextChangeTest.h"
#include "TextChange.h"
#include "Assert.h"

static void SimpleTests()
{
	{
		TextChange change;
		Assert( change.start == 0 );
		Assert( change.end == 0 );
		Assert( change.delta == 0 );
		change.AddChange( TextChange() );
		Assert( change.start == 0 );
		Assert( change.end == 0 );
		Assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 5, TextChange::insertion ) );
		Assert( change.start == 1 );
		Assert( change.end == 1 );
		Assert( change.delta == 5 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 5, TextChange::deletion ) );
		Assert( change.start == 1 );
		Assert( change.end == 6 );
		Assert( change.delta == -5 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 5, TextChange::modification ) );
		Assert( change.start == 1 );
		Assert( change.end == 6 );
		Assert( change.delta == 0 );
	}
}

static void ComplicatedTests()
{
	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		Assert( change.start == 1 );
		Assert( change.end == 1 );
		Assert( change.delta == 6 );
	}
	
	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::deletion ) );
		change.AddChange( TextChange( 1, 3, TextChange::deletion ) );
		Assert( change.start == 1 );
		Assert( change.end == 7 );
		Assert( change.delta == -6 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::modification ) );
		change.AddChange( TextChange( 1, 3, TextChange::modification  ) );
		Assert( change.start == 1 );
		Assert( change.end == 4 );
		Assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::modification ) );
		change.AddChange( TextChange( 0, 10, TextChange::modification ) );
		Assert( change.start == 0 );
		Assert( change.end == 10 );
		Assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 1, 3, TextChange::deletion ) );
		Assert( change.start == 1 );
		Assert( change.end == 1 );
		Assert( change.delta == 0 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 9, 3, TextChange::insertion ) );
		Assert( change.start == 1 );
		Assert( change.end == 6 );
		Assert( change.delta == 6 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 9, 3, TextChange::insertion ) );
		change.AddChange( TextChange( 1, 3, TextChange::insertion ) );
		Assert( change.start == 1 );
		Assert( change.end == 9 );
		Assert( change.delta == 6 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 9, TextChange::insertion ) );
		change.AddChange( TextChange( 2, 1, TextChange::insertion ) );
		Assert( change.start == 1 );
		Assert( change.end == 1 );
		Assert( change.delta == 10 );
	}

	{
		TextChange change;
		change.AddChange( TextChange( 1, 9, TextChange::deletion ) );
		change.AddChange( TextChange( 2, 1, TextChange::deletion ) );
		Assert( change.start == 1 );
		Assert( change.end == 12 );
		Assert( change.delta == -10 );
	}
}

void TextChangeTest()
{
	SimpleTests();
	ComplicatedTests();
}
