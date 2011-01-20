// VisualSelection.cpp

#include "VisualSelection.h"
#include "VisualPainter.h"
#include "TextStyleRegistry.h"

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

void VisualSelection::Draw( VisualPainter& painter, RECT rect )
{
	for ( Ranges::const_iterator it = m_ranges.begin(); it != m_ranges.end(); it += 2 )
	{
		LONG start = *it;
		LONG end   = *( it + 1 );

		RECT selectionRect;

		selectionRect.left   = start;
		selectionRect.top    = rect.top;
		selectionRect.right  = end;
		selectionRect.bottom = rect.top + painter.styleRegistry.lineHeight;

		FillRect( painter.hdc, &selectionRect, painter.styleRegistry.selectionBrush );
	}
}
