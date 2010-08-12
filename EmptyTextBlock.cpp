// EmptyTextBlock.cpp

#include "EmptyTextBlock.h"
#include "VisualPainter.h"
#include "TextStyle.h"

EmptyTextBlock::EmptyTextBlock( bool endsWithNewline, TextStyle& style )
	: m_endsWithNewline( endsWithNewline )
	, m_style( style )
{
}

void EmptyTextBlock::Draw( VisualPainter& painter, RECT rect ) const
{
	if ( painter.selection.Intersects( 0, Length() ) )
	{
		VisualSelection selection;
		selection.Add( 0, m_style.avgCharWidth );
		selection.Draw( painter, rect );
	}
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
	return m_style.lineHeight;
}

bool EmptyTextBlock::EndsWithNewline() const
{
	return m_endsWithNewline;
}
