// EmptyTextBlock.cpp

#include "EmptyTextBlock.h"
#include "VisualPainter.h"
#include "VisualSelection.h"
#include "TextStyleRegistry.h"

EmptyTextBlock::EmptyTextBlock( bool endsWithNewline, TextStyleRegistry& styleRegistry )
	: m_endsWithNewline( endsWithNewline )
	, m_styleRegistry( styleRegistry )
{
}

void EmptyTextBlock::DrawBackground( VisualPainter& painter, RECT rect ) const
{
	if ( painter.selection.Intersects( 0, Length() ) )
	{
		RECT drawRect = { 0,
		                  rect.top,
		                  m_styleRegistry.AvgCharWidth(),
		                  rect.top + m_styleRegistry.LineHeight() };

		painter.FillRect( drawRect, m_styleRegistry.Theme().selectionColor );
	}
}

void EmptyTextBlock::DrawText( VisualPainter& painter, RECT rect ) const
{
}

size_t EmptyTextBlock::LineCount() const
{
	return 1;
}

size_t EmptyTextBlock::LineContaining( size_t ) const
{
	return 0;
}

size_t EmptyTextBlock::LineStart( int y ) const
{
	return 0;
}

size_t EmptyTextBlock::LineEnd( int y ) const
{
	return 0;
}

POINT EmptyTextBlock::PointFromChar( size_t, bool advancing ) const
{
	POINT point = { 0, 0 };
	return point;
}

size_t EmptyTextBlock::CharFromPoint( POINT* point ) const
{
	point->x = 0;
	point->y = 0;
	return 0;
}

size_t EmptyTextBlock::Length() const
{
	return m_endsWithNewline ? 1 : 0;
}

int EmptyTextBlock::Height() const
{
	return m_styleRegistry.LineHeight();
}

bool EmptyTextBlock::EndsWithNewline() const
{
	return m_endsWithNewline;
}
