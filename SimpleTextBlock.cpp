// SimpleTextBlock.cpp

#include "SimpleTextBlock.h"
#include "VisualPainter.h"
#include "TextStyleRegistry.h"
#include "TextDocumentReader.h"
#include "TextStyleReader.h"
#include <algorithm>

SimpleTextBlock::SimpleTextBlock( SimpleLayoutDataPtr data, const TextStyleRegistry& styleRegistry )
	: m_data( std::move( data ) )
	, m_styleRegistry( styleRegistry )
{
}

void SimpleTextBlock::Draw( VisualPainter& painter, RECT rect ) const
{
	size_t firstLine = rect.top / m_styleRegistry.lineHeight;
	rect.top = firstLine * m_styleRegistry.lineHeight;

	RECT clipRect = rect;
	for ( size_t line = firstLine; line < m_data->lines.size() && !IsRectEmpty( &clipRect ); ++line )
	{
		DrawLineBackground( line, painter, clipRect );
		DrawLineSelection ( line, painter, clipRect );
		clipRect.top += m_styleRegistry.lineHeight;
	}

	clipRect = rect;
	for ( size_t line = firstLine; line < m_data->lines.size() && !IsRectEmpty( &clipRect ); ++line )
	{
		DrawLineText( line, painter, clipRect );
		clipRect.top += m_styleRegistry.lineHeight;
	}
}

void SimpleTextBlock::DrawLineBackground( size_t line, VisualPainter& painter, RECT rect ) const
{
	size_t pos = TextStart( line );
	int xStart = 0;

	ArrayOf<const TextStyleRun> styles = painter.styleReader.Styles( pos, TextEnd( line ) - pos );
	for ( const TextStyleRun* it = styles.begin(); it != styles.end() && xStart < rect.right; ++it )
	{
		int xEnd = CPtoX( line, pos + it->count - 1, true );

		DrawLineRect( painter, rect, xStart, xEnd, m_styleRegistry.Style( it->styleid ).bkColor );

		pos += it->count;
		xStart = xEnd;
	}
}

void SimpleTextBlock::DrawLineSelection( size_t line, VisualPainter& painter, RECT rect ) const
{
	if ( painter.selection.Intersects( TextStart( line ), TextEnd( line ) ) )
	{
		size_t start = std::max( painter.selection.Min(), TextStart( line ) );
		size_t end   = std::min( painter.selection.Max(), TextEnd  ( line ) );

		if ( start < end )
		{
			int xStart = CPtoX( line, start, false );
			int xEnd   = CPtoX( line, end - 1, true );

			DrawLineRect( painter, rect, xStart, xEnd, m_styleRegistry.selectionColor );
		}
	}

	if ( line == m_data->lines.size() - 1
	  && painter.selection.Intersects( TextEnd( line ), m_data->length )
	  && m_data->endsWithNewline )
	{
		int xStart = LineWidth( line );
		int xEnd   = LineWidth( line ) + m_styleRegistry.avgCharWidth;

		DrawLineRect( painter, rect, xStart, xEnd, m_styleRegistry.selectionColor );
	}
}

void SimpleTextBlock::DrawLineText( size_t line, VisualPainter& painter, RECT rect ) const
{
	size_t pos = TextStart( line );
	int xStart = 0;

	ArrayOf<const TextStyleRun> styles = painter.styleReader.Styles( pos, TextEnd( line ) - pos );
	for ( const TextStyleRun* it = styles.begin(); it != styles.end() && xStart < rect.right; ++it )
	{
		const TextStyle& style = m_styleRegistry.Style( it->styleid );
		SelectObject( painter.hdc, m_styleRegistry.Font( style.fontid ).hfont );
		SetTextColor( painter.hdc, style.textColor );

		UTF16Ref text = painter.docReader.StrictRange( painter.textStart + pos, it->count );
		ExtTextOutW( painter.hdc, xStart, rect.top, ETO_CLIPPED, &rect, text.begin(), it->count, NULL );

		pos += it->count;
		xStart = CPtoX( line, pos - 1, true );
	}
}

void SimpleTextBlock::DrawLineRect( VisualPainter& painter, RECT rect, int xStart, int xEnd, uint32 color ) const
{
	RECT drawRect = { std::max<int>( xStart, rect.left ),
	                  rect.top,
	                  std::min<int>( xEnd, rect.right ),
	                  rect.top + m_styleRegistry.lineHeight };

	if ( !IsRectEmpty( &drawRect ) )
		painter.DrawRect( drawRect, color );
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
	int line = y / m_styleRegistry.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextStart( line );
}

size_t SimpleTextBlock::LineEnd( int y ) const
{
	int line = y / m_styleRegistry.lineHeight;
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
	result.y = line * m_styleRegistry.lineHeight;

	return result;
}

size_t SimpleTextBlock::CharFromPoint( POINT* point ) const
{
	int line = point->y / m_styleRegistry.lineHeight;

	if ( line < 0 || size_t( line ) >= m_data->lines.size() )
		line = m_data->lines.size() - 1;

	point->y = line * m_styleRegistry.lineHeight;
	return XtoCP( line, &point->x );
}

size_t SimpleTextBlock::Length() const
{
	return m_data->length;
}

int SimpleTextBlock::Height() const
{
	return m_data->lines.size() * m_styleRegistry.lineHeight;
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
