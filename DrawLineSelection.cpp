// DrawLineSelection.cpp

#include "DrawLineSelection.h"
#include "SelectionLineLoop.h"

static void VertLine( HDC hdc, int x, int y1, int y2 )
{
	MoveToEx( hdc, x, y1, NULL );
	LineTo  ( hdc, x, y2 );
}

static void HorizLine( HDC hdc, int x1, int x2, int y )
{
	MoveToEx( hdc, x1, y, NULL );
	LineTo  ( hdc, x2, y );
}

void DrawHorizontalLine( HDC hdc, int x1, int x2, int y, bool isTop )
{
	static HPEN innerTop = CreatePen( PS_SOLID, 1, RGB( 117, 186, 255 ) );
	static HPEN outerTop = CreatePen( PS_SOLID, 1, RGB( 229, 242, 255 ) );

	SelectObject( hdc, innerTop );
	HorizLine( hdc, x1, x2 + 1, ( isTop ? y - 1 : y ) );

	if ( x2 - x1 > 1 )
	{
		SelectObject( hdc, outerTop );
		HorizLine( hdc, x1 + 1, x2, ( isTop ? y : y - 1 ) );
	}
}

void DrawHorizontalLines( HDC hdc, RECT lineRect, const SelectionRanges& top, const SelectionRanges& bottom )
{
	for ( SelectionLineLoop line( top, bottom ); line.Unfinished(); ++line )
		if ( lineRect.left < line.End() && line.Start() < lineRect.right )
			DrawHorizontalLine( hdc, line.Start(), line.End(), lineRect.top, line.IsTop() );
}

static void DrawVerticalLines( HDC hdc, int x1, int x2, RECT lineRect )
{
	static HPEN innerSides = CreatePen( PS_SOLID, 1, RGB(  73, 163, 255 ) );
	static HPEN outerSides = CreatePen( PS_SOLID, 1, RGB( 229, 242, 255 ) );

	int yTop    = lineRect.top;
	int yBottom = lineRect.bottom;

	SelectObject( hdc, innerSides );
	VertLine( hdc, x1, yTop, yBottom );
	VertLine( hdc, x2, yTop, yBottom );

	if ( yBottom - yTop > 2 )
	{
		SelectObject( hdc, outerSides );
		VertLine( hdc, x1 - 1, yTop + 1, yBottom - 1 );
		VertLine( hdc, x2 + 1, yTop + 1, yBottom - 1 );
	}
}

void DrawLineSelection( HDC hdc, RECT lineRect, const SelectionRanges& top, const SelectionRanges& bottom, bool isLast )
{
	if ( top.empty() && bottom.empty() )
		return;

	for ( SelectionRanges::const_iterator it = bottom.begin(); it != bottom.end(); ++it )
	{
		RECT selRect = { it->first, lineRect.top, it->second, lineRect.bottom };

		RECT intersection;
		if ( IntersectRect( &intersection, &lineRect, &selRect ) )
		{
			static HBRUSH fill = CreateSolidBrush( RGB( 215, 235, 255 ) );
			FillRect( hdc, &intersection, fill );
			DrawVerticalLines( hdc, it->first, it->second, lineRect );
		}
	}

	DrawHorizontalLines( hdc, lineRect, top, bottom );

	if ( isLast )
	{
		OffsetRect( &lineRect, 0, lineRect.bottom - lineRect.top );
		DrawHorizontalLines( hdc, lineRect, bottom, SelectionRanges() );
	}
}
