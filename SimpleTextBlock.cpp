// SimpleTextBlock.cpp

#include "SimpleTextBlock.h"
#include "VisualPainter.h"
#include "VisualSelection.h"
#include "DocumentReader.h"
#include "TextStyle.h"

SimpleTextBlock::SimpleTextBlock( SimpleLayoutDataPtr data, TextStyle& style )
	: m_data( std::move( data ) )
	, m_style( style )
{
}

void SimpleTextBlock::Draw( VisualPainter& painter, RECT rect ) const
{
	size_t line = rect.top / m_style.lineHeight;
	rect.top = line * m_style.lineHeight;

	for ( ; line < m_data->lines.size() && !IsRectEmpty( &rect ); ++line )
	{
		DrawLineSelection( line, painter, rect );
		DrawLineText     ( line, painter, rect );

		rect.top += m_style.lineHeight;
	}
}

void SimpleTextBlock::DrawLineSelection( size_t line, VisualPainter& painter, RECT rect ) const
{
	VisualSelection selection;

	if ( painter.selection.Intersects( TextStart( line ), TextEnd( line ) ) )
	{
		size_t start = std::max( painter.selection.Min(), TextStart( line ) );
		size_t end   = std::min( painter.selection.Max(), TextEnd  ( line ) );

		if ( start < end )
			selection.Add( CPtoX( line, start, false ), CPtoX( line, end - 1, true ) );
	}

	if ( line == m_data->lines.size() - 1
	  && painter.selection.Intersects( TextEnd( line ), m_data->length )
	  && m_data->endsWithNewline )
	{
		selection.Add( LineWidth( line ), LineWidth( line ) + painter.style.avgCharWidth );
	}

	selection.Draw( painter, rect );
}

void SimpleTextBlock::DrawLineText( size_t line, VisualPainter& painter, RECT rect ) const
{
	ArrayOf<const SimpleTextRun> runs = LineRuns( line );

	DocumentReader reader( painter.doc );

	for ( const SimpleTextRun* run = runs.begin(); run != runs.end() && !IsRectEmpty( &rect ); ++run )
	{
		painter.SetFont( run->font );

		ExtTextOutW( painter.hdc,
		             rect.left,
		             rect.top,
		             ETO_CLIPPED,
		             &rect,
		             reader.StrictRange( painter.textStart + run->textStart, run->textCount ).begin(),
		             run->textCount,
		             NULL );

		rect.left += RunWidth( run );
	}
}

size_t SimpleTextBlock::LineCount() const
{
	return m_data->lines.size();
}

size_t SimpleTextBlock::LineContaining( size_t pos ) const
{
	size_t line = 0;
	while ( line < m_data->lines.size() && TextEnd( line ) <= pos )
		++line;
	return line;
}

size_t SimpleTextBlock::LineStart( int y ) const
{
	int line = y / m_style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextStart( line );
}

size_t SimpleTextBlock::LineEnd( int y ) const
{
	int line = y / m_style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextEnd( line );
}

POINT SimpleTextBlock::PointFromChar( size_t pos, bool advancing ) const
{
	POINT result;
	bool trailingEdge = advancing;

	if ( pos == 0 )
		trailingEdge = false;
	else if ( pos >= TextEnd( m_data->lines.size() - 1 ) )
		trailingEdge = true;

	if ( trailingEdge )
		--pos;

	size_t line = LineContaining( pos );
	Assert( line < m_data->lines.size() );

	result.x = CPtoX( line, pos, trailingEdge );
	result.y = line * m_style.lineHeight;

	return result;
}

size_t SimpleTextBlock::CharFromPoint( POINT* point ) const
{
	int line = point->y / m_style.lineHeight;

	if ( line < 0 || size_t( line ) >= m_data->lines.size() )
		line = m_data->lines.size() - 1;

	point->y = line * m_style.lineHeight;
	return XtoCP( line, &point->x );
}

size_t SimpleTextBlock::Length() const
{
	return m_data->length;
}

int SimpleTextBlock::Height() const
{
	return m_data->lines.size() * m_style.lineHeight;
}

bool SimpleTextBlock::EndsWithNewline() const
{
	return m_data->endsWithNewline;
}

ArrayOf<const SimpleTextRun> SimpleTextBlock::LineRuns( size_t line ) const
{
	Assert( line < m_data->lines.size() );
	const SimpleTextRun* runStart = &m_data->runs.front() + ( line > 0 ? m_data->lines[line - 1] : 0 );
	return ArrayOf<const SimpleTextRun>( runStart, &m_data->runs.front() + m_data->lines[line] );
}

size_t SimpleTextBlock::TextStart( size_t line ) const
{
	ArrayOf<const SimpleTextRun> runs = LineRuns( line );
	return runs[0].textStart;
}

size_t SimpleTextBlock::TextEnd( size_t line ) const
{
	ArrayOf<const SimpleTextRun> runs = LineRuns( line );
	return runs[runs.size() - 1].textStart + runs[runs.size() - 1].textCount;
}

int SimpleTextBlock::LineWidth( size_t line ) const
{
	ArrayOf<const SimpleTextRun> runs = LineRuns( line );

	int width = 0;
	for ( const SimpleTextRun* run = runs.begin(); run != runs.end(); ++run )
		width += RunWidth( run );
	return width;
}

int SimpleTextBlock::RunWidth( const SimpleTextRun* run ) const
{
	return m_data->xOffsets[run->textStart + run->textCount - 1];
}

int SimpleTextBlock::CPtoX( size_t line, size_t cp, bool trailingEdge ) const
{
	ArrayOf<const SimpleTextRun> runs = LineRuns( line );

	int x = 0;
	const SimpleTextRun* run = runs.begin();

	while ( run != runs.end() && run->textStart + run->textCount <= cp )
	{
		x += RunWidth( run );
		run++;
	}

	Assert( run != runs.end() );
	Assert( cp < run->textStart + run->textCount );

	if ( trailingEdge )
		x += m_data->xOffsets[cp];
	else if ( cp > run->textStart )
		x += m_data->xOffsets[cp - 1];
	
	return x;
}

size_t SimpleTextBlock::XtoCP( size_t line, LONG* x ) const
{
	ArrayOf<const SimpleTextRun> runs = LineRuns( line );

	int xStart = 0;
	const SimpleTextRun* run = runs.begin();

	while ( run != runs.end() && xStart + RunWidth( run ) <= *x )
	{
		xStart += RunWidth( run );
		run++;
	}

	if ( run == runs.end() )
	{
		*x = LineWidth( line );
		return TextEnd( line );
	}

	size_t cp = run->textStart;
	int xOffset = 0;

	while ( cp < run->textStart + run->textCount && xStart + xOffset + ( m_data->xOffsets[cp] - xOffset ) / 2 < *x )
	{
		xOffset = m_data->xOffsets[cp];
		++cp;
	}

	*x = xStart + xOffset;
	return cp;
}
