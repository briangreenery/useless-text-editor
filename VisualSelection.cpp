// VisualSelection.cpp

#include "VisualSelection.h"
#include "VisualPainter.h"
#include <algorithm>

#undef min
#undef max

void VisualSelection::Add( LONG xStart, LONG xEnd )
{
	if ( m_ranges.empty() || m_ranges.back() != xStart )
	{
		m_ranges.push_back( xStart );
		m_ranges.push_back( xEnd );
	}
	else
	{
		m_ranges.back() = xEnd;
	}
}

void VisualSelection::Swap( VisualSelection& other )
{
	m_ranges.swap( other.m_ranges );
}

void VisualSelection::Draw( VisualPainter& painter, RECT rect )
{
	for ( Ranges::const_iterator it = m_ranges.begin(); it != m_ranges.end(); it += 2 )
	{
		LONG start = *it;
		LONG end   = *( it + 1 );

		RECT selectionRect;

		selectionRect.left   = std::max( rect.left,   start );
		selectionRect.top    = rect.top;
		selectionRect.right  = std::min( rect.right,  end );
		selectionRect.bottom = rect.top + painter.style.lineHeight;

		FillRect( painter.hdc, &selectionRect, painter.style.selectionBrush );

		DrawHighlight( painter, start, end, painter.prevSelection.m_ranges, rect.top - 1 );
		DrawHighlight( painter, start, end, Ranges(),                       rect.top + painter.style.lineHeight );
	}
}

void VisualSelection::DrawHighlight( VisualPainter& painter, LONG start, LONG end, const Ranges& above, LONG y )
{
	Ranges::const_iterator it = above.begin();

	while ( start < end )
	{
		while ( it != above.end() && *it <= start )
			++it;

		LONG aboveEnd = std::min( end, ( ( it != above.end() ) ? *it : MAXLONG ) );

		if ( ( it - above.begin() ) % 2 == 0 )
		{
			MoveToEx( painter.hdc, start,    y, NULL );
			LineTo  ( painter.hdc, aboveEnd, y );
		}

		start = aboveEnd;
	}
}
