// SimpleTextBlock.cpp

#include "SimpleTextBlock.h"
#include "VisualPainter.h"
#include "TextStyleRegistry.h"
#include "TextDocumentReader.h"
#include "TextStyleReader.h"
#include "CharBuffer.h"
#include <algorithm>

SimpleTextBlock::SimpleTextBlock( SimpleLayoutDataPtr data, const TextStyleRegistry& styleRegistry )
	: m_data( std::move( data ) )
	, m_styleRegistry( styleRegistry )
{
}

void SimpleTextBlock::DrawBackground( VisualPainter& painter, RECT rect ) const
{
	size_t firstLine = rect.top / m_styleRegistry.LineHeight();
	rect.top = firstLine * m_styleRegistry.LineHeight();

	for ( size_t line = firstLine; line < m_data->lines.size() && !IsRectEmpty( &rect ); ++line )
	{
		DrawLineBackground( line, painter, rect );
		DrawLineSelection ( line, painter, rect );
		DrawLineSquiggles ( line, painter, rect );
		DrawLineHighlights( line, painter, rect );
		rect.top += m_styleRegistry.LineHeight();
	}
}

void SimpleTextBlock::DrawText( VisualPainter& painter, RECT rect ) const
{
	size_t firstLine = rect.top / m_styleRegistry.LineHeight();
	rect.top = firstLine * m_styleRegistry.LineHeight();

	for ( size_t line = firstLine; line < m_data->lines.size() && !IsRectEmpty( &rect ); ++line )
	{
		DrawLineText( line, painter, rect );
		rect.top += m_styleRegistry.LineHeight();
	}
}

void SimpleTextBlock::DrawLineBackground( size_t line, VisualPainter& painter, RECT rect ) const
{
	size_t pos = TextStart( line );
	int xStart = 0;

	ArrayRef<const TextStyleRun> styles = painter.styleReader.Classes( painter.textStart + pos, TextEnd( line ) - pos );
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
		size_t start = (std::max)( painter.selection.Min(), TextStart( line ) );
		size_t end   = (std::min)( painter.selection.Max(), TextEnd  ( line ) );

		int xStart = CPtoX( line, start, false );
		int xEnd   = CPtoX( line, end - 1, true );

		DrawLineRect( painter, rect, xStart, xEnd, m_styleRegistry.Theme().selectionColor );
	}

	if ( line == m_data->lines.size() - 1
	  && painter.selection.Intersects( TextEnd( line ), m_data->length )
	  && m_data->endsWithNewline )
	{
		int xStart = LineWidth( line );
		int xEnd   = LineWidth( line ) + m_styleRegistry.AvgCharWidth();

		DrawLineRect( painter, rect, xStart, xEnd, m_styleRegistry.Theme().selectionColor );
	}
}

void SimpleTextBlock::DrawLineSquiggles( size_t line, VisualPainter& painter, RECT rect ) const
{
	size_t textStart = TextStart( line );
	size_t textEnd   = TextEnd  ( line );

	ArrayRef<const CharRange> squiggles = painter.styleReader.Squiggles( painter.textStart + textStart, textEnd - textStart );
	for ( const CharRange* it = squiggles.begin(); it != squiggles.end(); ++it )
	{
		int xStart = CPtoX( line, it->start - painter.textStart,                 false );
		int xEnd   = CPtoX( line, it->start - painter.textStart + it->count - 1, true );

		if ( xStart >= rect.right )
			break;

		painter.DrawSquiggles( xStart, xEnd, rect );
	}
}

void SimpleTextBlock::DrawLineHighlights( size_t line, VisualPainter& painter, RECT rect ) const
{
	size_t textStart = TextStart( line );
	size_t textEnd   = TextEnd( line );

	ArrayRef<const CharRange> highlights = painter.styleReader.Highlights( painter.textStart + textStart, textEnd - textStart );
	for ( const CharRange* it = highlights.begin(); it != highlights.end(); ++it )
	{
		int xStart = CPtoX( line, it->start - painter.textStart,                 false );
		int xEnd   = CPtoX( line, it->start - painter.textStart + it->count - 1, true );

		if ( xStart >= rect.right )
			break;

		painter.DrawHighlight( xStart, xEnd, rect );
	}
}

void SimpleTextBlock::DrawLineText( size_t line, VisualPainter& painter, RECT rect ) const
{
	size_t lineStart = TextStart( line );
	size_t lineEnd = TextEnd( line );

	ArrayRef<const SimpleTextRun> runs = LineRuns( line );
	ArrayRef<const TextStyleRun> styles = painter.styleReader.Classes( painter.textStart + lineStart, lineEnd - lineStart );

	int xRunStart = 0;
	for ( const SimpleTextRun* run = runs.begin(); run != runs.end() && xRunStart < rect.right; ++run )
	{
		ArrayRef<const wchar_t> text = painter.charBuffer.Read( painter.textStart + run->textStart, run->textCount );

		SelectObject( painter.hdc, m_styleRegistry.Font( run->fontid ).hfont );

		ArrayRef<const TextStyleRun> runStyles = RunStyles( painter.textStart, *run, styles );
		for ( const TextStyleRun* style = runStyles.begin(); style != runStyles.end(); ++style )
		{
			size_t overlapStart = (std::max)( style->start - painter.textStart,                run->textStart );
			size_t overlapEnd   = (std::min)( style->start - painter.textStart + style->count, run->textStart + run->textCount );

			int xStart = xRunStart + RunCPtoX( *run, overlapStart, false );
			if ( xStart >= rect.right )
				break;

			painter.SetTextColor( m_styleRegistry.Style( style->styleid ).textColor );
			ExtTextOutW( painter.hdc,
			             xStart,
			             rect.top,
			             ETO_CLIPPED,
			             &rect,
			             text.begin() + ( overlapStart - run->textStart ),
			             overlapEnd - overlapStart,
			             NULL );
		}

		xRunStart += RunWidth( run );
	}
}

ArrayRef<const TextStyleRun> SimpleTextBlock::RunStyles( size_t blockStart, const SimpleTextRun& run, ArrayRef<const TextStyleRun> styles ) const
{
	struct StyleRunEqual
	{
		StyleRunEqual( size_t blockStart ) : m_blockStart( blockStart ) {}
		bool operator()( const TextStyleRun&  a, const TextStyleRun&  b ) const { return a.start + a.count < b.start; }
		bool operator()( const TextStyleRun&  a, const SimpleTextRun& b ) const { return a.start + a.count <= m_blockStart + b.textStart; }
		bool operator()( const SimpleTextRun& a, const TextStyleRun&  b ) const { return m_blockStart + a.textStart + a.textCount <= b.start; }
		size_t m_blockStart;
	};

	std::pair<const TextStyleRun*, const TextStyleRun*> range = std::equal_range( styles.begin(), styles.end(), run, StyleRunEqual( blockStart ) );

	return ArrayRef<const TextStyleRun>( range.first, range.second );
}

void SimpleTextBlock::DrawLineRect( VisualPainter& painter, RECT rect, int xStart, int xEnd, COLORREF color ) const
{
	if ( color == TextStyle::useDefault )
		return;

	RECT drawRect = { std::max<int>( xStart, rect.left ),
	                  rect.top,
	                  std::min<int>( xEnd, rect.right ),
	                  rect.top + m_styleRegistry.LineHeight() };

	if ( !IsRectEmpty( &drawRect ) )
		painter.FillRect( drawRect, color );
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
	int line = y / m_styleRegistry.LineHeight();
	assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextStart( line );
}

size_t SimpleTextBlock::LineEnd( int y ) const
{
	int line = y / m_styleRegistry.LineHeight();
	assert( line >= 0 && size_t( line ) < m_data->lines.size() );
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
	assert( line < m_data->lines.size() );

	result.x = CPtoX( line, pos, trailingEdge );
	result.y = line * m_styleRegistry.LineHeight();

	return result;
}

size_t SimpleTextBlock::CharFromPoint( POINT* point ) const
{
	int line = point->y / m_styleRegistry.LineHeight();

	if ( line < 0 || size_t( line ) >= m_data->lines.size() )
		line = m_data->lines.size() - 1;

	point->y = line * m_styleRegistry.LineHeight();
	return XtoCP( line, &point->x );
}

size_t SimpleTextBlock::Length() const
{
	return m_data->length;
}

int SimpleTextBlock::Height() const
{
	return m_data->lines.size() * m_styleRegistry.LineHeight();
}

bool SimpleTextBlock::EndsWithNewline() const
{
	return m_data->endsWithNewline;
}

ArrayRef<const SimpleTextRun> SimpleTextBlock::LineRuns( size_t line ) const
{
	assert( line < m_data->lines.size() );
	const SimpleTextRun* runStart = &m_data->runs[line == 0 ? 0 : m_data->lines[line - 1]];
	return ArrayRef<const SimpleTextRun>( runStart, &m_data->runs.front() + m_data->lines[line] );
}

size_t SimpleTextBlock::TextStart( size_t line ) const
{
	ArrayRef<const SimpleTextRun> runs = LineRuns( line );
	return runs[0].textStart;
}

size_t SimpleTextBlock::TextEnd( size_t line ) const
{
	ArrayRef<const SimpleTextRun> runs = LineRuns( line );
	return runs[runs.size() - 1].textStart + runs[runs.size() - 1].textCount;
}

int SimpleTextBlock::LineWidth( size_t line ) const
{
	ArrayRef<const SimpleTextRun> runs = LineRuns( line );

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
	ArrayRef<const SimpleTextRun> runs = LineRuns( line );

	int x = 0;
	const SimpleTextRun* run = runs.begin();

	while ( run != runs.end() && run->textStart + run->textCount <= cp )
	{
		x += RunWidth( run );
		run++;
	}

	assert( run != runs.end() );
	return x + RunCPtoX( *run, cp, trailingEdge );
}

int SimpleTextBlock::RunCPtoX( const SimpleTextRun& run, size_t cp, bool trailingEdge ) const
{
	assert( cp >= run.textStart );
	assert( cp < run.textStart + run.textCount );

	if ( trailingEdge )
		return m_data->xOffsets[cp];
	if ( cp == run.textStart )
		return 0;
	return m_data->xOffsets[cp - 1];
}

size_t SimpleTextBlock::XtoCP( size_t line, LONG* x ) const
{
	ArrayRef<const SimpleTextRun> runs = LineRuns( line );

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
