// UniscribeTextBlock.cpp

#include "UniscribeTextBlock.h"
#include "VisualPainter.h"
#include "TextStyleRegistry.h"
#include "ThrowHRESULT.h"
#include <cassert>
#include <algorithm>

UniscribeTextBlock::UniscribeTextBlock( UniscribeLayoutDataPtr data, const TextStyleRegistry& styleRegistry )
	: m_data( std::move( data ) )
	, m_styleRegistry( styleRegistry )
{
}

void UniscribeTextBlock::DrawBackground( VisualPainter& painter, RECT rect ) const
{
	size_t firstLine = rect.top / m_styleRegistry.LineHeight();
	rect.top = firstLine * m_styleRegistry.LineHeight();

	for ( size_t line = firstLine; line < m_data->lines.size() && !IsRectEmpty( &rect ); ++line )
	{
		std::vector<int> visualToLogical = VisualToLogicalMapping( LineRuns( line ) );

		DrawLineBackground( line, visualToLogical, painter, rect );
		DrawLineSelection ( line, visualToLogical, painter, rect );
		DrawLineSquiggles ( line, visualToLogical, painter, rect );
		DrawLineHighlights( line, visualToLogical, painter, rect );
		rect.top += m_styleRegistry.LineHeight();
	}
}

void UniscribeTextBlock::DrawText( VisualPainter& painter, RECT rect ) const
{
	size_t firstLine = rect.top / m_styleRegistry.LineHeight();
	rect.top = firstLine * m_styleRegistry.LineHeight();

	for ( size_t line = firstLine; line < m_data->lines.size() && !IsRectEmpty( &rect ); ++line )
	{
		std::vector<int> visualToLogical = VisualToLogicalMapping( LineRuns( line ) );

		DrawLineText( line, visualToLogical, painter, rect );
		rect.top += m_styleRegistry.LineHeight();
	}
}

void UniscribeTextBlock::DrawLineBackground( size_t line, const std::vector<int>& visualToLogical, VisualPainter& painter, RECT rect ) const
{
	size_t lineStart = TextStart( line );
	size_t lineEnd   = TextEnd( line );

	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	ArrayRef<const TextStyleRun> styles = painter.styleReader.Classes( painter.textStart + lineStart, lineEnd - lineStart );

	int xStart = 0;
	for ( size_t i = 0; i < visualToLogical.size() && xStart < rect.right; ++i )
	{
		const UniscribeTextRun& run = runs[visualToLogical[i]];

		ArrayRef<const TextStyleRun> runStyles = RunStyles( painter.textStart, run, styles );
		for ( const TextStyleRun* style = runStyles.begin(); style != runStyles.end(); ++style )
		{
			size_t styleStart = style->start - painter.textStart;
			size_t styleEnd   = style->start - painter.textStart + style->count;

			std::pair<int,int> range = RunCPtoXRange( run, styleStart, styleEnd );

			DrawLineRect( painter,
			              rect,
			              xStart + range.first,
			              xStart + range.second,
			              m_styleRegistry.Style( style->styleid ).bkColor );
		}

		xStart += run.width;
	}
}

void UniscribeTextBlock::DrawLineSelection( size_t line, const std::vector<int>& visualToLogical, VisualPainter& painter, RECT rect ) const
{
	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );

	if ( painter.selection.Intersects( TextStart( line ), TextEnd( line ) ) )
	{
		int xStart = 0;
		for ( size_t i = 0; i < visualToLogical.size() && xStart < rect.right; ++i )
		{
			const UniscribeTextRun& run = runs[visualToLogical[i]];

			std::pair<int,int> range = RunCPtoXRange( run, painter.selection.Min(), painter.selection.Max() );

			DrawLineRect( painter,
			              rect,
			              xStart + range.first,
			              xStart + range.second,
			              m_styleRegistry.Theme().selectionColor );

			xStart += run.width;
		}
	}

	if ( line == m_data->lines.size() - 1
	  && painter.selection.Intersects( TextEnd( line ), m_data->length )
	  && m_data->endsWithNewline )
	{
		DrawLineRect( painter,
		              rect,
		              LineWidth( line ),
		              LineWidth( line ) + m_styleRegistry.AvgCharWidth(),
		              m_styleRegistry.Theme().selectionColor );
	}
}

void UniscribeTextBlock::DrawLineSquiggles( size_t line, const std::vector<int>& visualToLogical, VisualPainter& painter, RECT rect ) const
{
	size_t lineStart = TextStart( line );
	size_t lineEnd   = TextEnd( line );

	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	ArrayRef<const TextRange> squiggles = painter.styleReader.Squiggles( painter.textStart + lineStart, lineEnd - lineStart );

	if ( squiggles.empty() )
		return;

	int xStart = 0;
	for ( size_t i = 0; i < visualToLogical.size() && xStart < rect.right; ++i )
	{
		const UniscribeTextRun& run = runs[visualToLogical[i]];

		ArrayRef<const TextRange> runSquiggles = RunSquiggles( painter.textStart, run, squiggles );
		for ( const TextRange* squiggle = runSquiggles.begin(); squiggle != runSquiggles.end(); ++squiggle )
		{
			size_t squiggleStart = squiggle->start - painter.textStart;
			size_t squiggleEnd   = squiggle->start - painter.textStart + squiggle->count;

			std::pair<int,int> range = RunCPtoXRange( run, squiggleStart, squiggleEnd );

			painter.DrawSquiggles( xStart + range.first, xStart + range.second, rect );
		}

		xStart += run.width;
	}
}

void UniscribeTextBlock::DrawLineHighlights( size_t line, const std::vector<int>& visualToLogical, VisualPainter& painter, RECT rect ) const
{
	size_t lineStart = TextStart( line );
	size_t lineEnd   = TextEnd( line );

	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	ArrayRef<const TextRange> highlights = painter.styleReader.Highlights( painter.textStart + lineStart, lineEnd - lineStart );

	if ( highlights.empty() )
		return;

	int xStart = 0;
	for ( size_t i = 0; i < visualToLogical.size() && xStart < rect.right; ++i )
	{
		const UniscribeTextRun& run = runs[visualToLogical[i]];

		ArrayRef<const TextRange> runHighlights = RunSquiggles( painter.textStart, run, highlights );
		for ( const TextRange* highlight = runHighlights.begin(); highlight != runHighlights.end(); ++highlight )
		{
			size_t highlightStart = highlight->start - painter.textStart;
			size_t highlightEnd   = highlight->start - painter.textStart + highlight->count;

			std::pair<int,int> range = RunCPtoXRange( run, highlightStart, highlightEnd );

			painter.DrawHighlight( xStart + range.first, xStart + range.second, rect );
		}

		xStart += run.width;
	}
}

void UniscribeTextBlock::DrawLineText( size_t line, const std::vector<int>& visualToLogical, VisualPainter& painter, RECT rect ) const
{
	size_t lineStart = TextStart( line );
	size_t lineEnd   = TextEnd( line );

	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	ArrayRef<const TextStyleRun> styles = painter.styleReader.Classes( painter.textStart + lineStart, lineEnd - lineStart );

	int xStart = 0;
	for ( size_t i = 0; i < visualToLogical.size() && xStart < rect.right; ++i )
	{
		const UniscribeTextRun& run = runs[visualToLogical[i]];

		const TextFont& font = m_styleRegistry.Font( run.fontid );
		SelectObject( painter.hdc, font.hfont );

		ArrayRef<const TextStyleRun> runStyles = RunStyles( painter.textStart, run, styles );
		for ( const TextStyleRun* style = runStyles.begin(); style != runStyles.end(); ++style )
		{
			size_t styleStart = style->start - painter.textStart;
			size_t styleEnd   = style->start - painter.textStart + style->count;

			std::pair<int,int> range = RunCPtoXRange( run, styleStart, styleEnd );

			// textRect has an extra 'lineHeight' in each direction to show overhangs and underhangs without clipping.
			RECT textRect = { std::max<int>( rect.left, xStart + range.first ),
			                  rect.top - m_styleRegistry.LineHeight(),
			                  std::min<int>( rect.right, xStart + range.second ),
			                  rect.top + 2*m_styleRegistry.LineHeight() };

			painter.SetTextColor( m_styleRegistry.Style( style->styleid ).textColor );
			ThrowHRESULT( ScriptTextOut( painter.hdc,
			                             &font.fontCache,
			                             xStart,
			                             rect.top,
			                             ETO_CLIPPED,
			                             &textRect,
			                             &m_data->items[run.item].a,
			                             0,
			                             0,
			                             &m_data->glyphs[run.glyphStart],
			                             run.glyphCount,
			                             &m_data->advanceWidths[run.glyphStart],
			                             NULL,
			                             &m_data->offsets[run.glyphStart] ) );
		}

		xStart += run.width;
	}
}

ArrayRef<const TextStyleRun> UniscribeTextBlock::RunStyles( size_t blockStart, const UniscribeTextRun& run, ArrayRef<const TextStyleRun> styles ) const
{
	struct StyleRunEqual
	{
		StyleRunEqual( size_t blockStart ) : m_blockStart( blockStart ) {}
		bool operator()( const TextStyleRun&     a, const TextStyleRun&     b ) const { return a.start + a.count <= b.start; }
		bool operator()( const TextStyleRun&     a, const UniscribeTextRun& b ) const { return a.start + a.count <= m_blockStart + b.textStart; }
		bool operator()( const UniscribeTextRun& a, const TextStyleRun&     b ) const { return m_blockStart + a.textStart + a.textCount <= b.start; }
		size_t m_blockStart;
	};

	std::pair<const TextStyleRun*, const TextStyleRun*> range = std::equal_range( styles.begin(), styles.end(), run, StyleRunEqual( blockStart ) );
	return ArrayRef<const TextStyleRun>( range.first, range.second );
}

ArrayRef<const TextRange> UniscribeTextBlock::RunSquiggles( size_t blockStart, const UniscribeTextRun& run, ArrayRef<const TextRange> squiggles ) const
{
	struct TextRangeEqual
	{
		TextRangeEqual( size_t blockStart ) : m_blockStart( blockStart ) {}
		bool operator()( const TextRange&        a, const TextRange&        b ) const { return a.start + a.count <= b.start; }
		bool operator()( const TextRange&        a, const UniscribeTextRun& b ) const { return a.start + a.count <= m_blockStart + b.textStart; }
		bool operator()( const UniscribeTextRun& a, const TextRange&        b ) const { return m_blockStart + a.textStart + a.textCount <= b.start; }
		size_t m_blockStart;
	};

	std::pair<const TextRange*, const TextRange*> range = std::equal_range( squiggles.begin(), squiggles.end(), run, TextRangeEqual( blockStart ) );
	return ArrayRef<const TextRange>( range.first, range.second );
}

void UniscribeTextBlock::DrawLineRect( VisualPainter& painter, RECT rect, int xStart, int xEnd, COLORREF color ) const
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

POINT UniscribeTextBlock::PointFromChar( size_t pos, bool advancing ) const
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

size_t UniscribeTextBlock::CharFromPoint( POINT* point ) const
{
	int line = point->y / m_styleRegistry.LineHeight();

	if ( line < 0 || size_t( line ) >= m_data->lines.size() )
		line = m_data->lines.size() - 1;

	point->y = line * m_styleRegistry.LineHeight();
	return XtoCP( line, &point->x );
}

size_t UniscribeTextBlock::LineStart( int y ) const
{
	int line = y / m_styleRegistry.LineHeight();
	assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextStart( line );
}

size_t UniscribeTextBlock::LineEnd( int y ) const
{
	int line = y / m_styleRegistry.LineHeight();
	assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextEnd( line );
}

size_t UniscribeTextBlock::LineCount() const
{
	return m_data->lines.size();
}

size_t UniscribeTextBlock::Length() const
{
	return m_data->length;
}

int UniscribeTextBlock::Height() const
{
	return m_data->lines.size() * m_styleRegistry.LineHeight();
}

bool UniscribeTextBlock::EndsWithNewline() const
{
	return m_data->endsWithNewline;
}

ArrayRef<const UniscribeTextRun> UniscribeTextBlock::LineRuns( size_t line ) const
{
	assert( line < m_data->lines.size() );
	const UniscribeTextRun* runStart = &m_data->runs[line == 0 ? 0 : m_data->lines[line - 1]];
	return ArrayRef<const UniscribeTextRun>( runStart, &m_data->runs.front() + m_data->lines[line] );
}

size_t UniscribeTextBlock::TextStart( size_t line ) const
{
	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	assert( !runs.empty() );
	return runs[0].textStart;
}

size_t UniscribeTextBlock::TextEnd( size_t line ) const
{
	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	assert( !runs.empty() );
	return runs[runs.size() - 1].textStart + runs[runs.size() - 1].textCount;
}

int UniscribeTextBlock::LineWidth( size_t line ) const
{
	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );

	int width = 0;
	for ( const UniscribeTextRun* run = runs.begin(); run != runs.end(); ++run )
		width += run->width;
	return width;
}

int UniscribeTextBlock::RunCPtoX( const UniscribeTextRun& run, size_t cp, bool trailingEdge ) const
{
	int x;
	ThrowHRESULT( ScriptCPtoX( cp,
	                           trailingEdge,
	                           run.textCount,
	                           run.glyphCount,
	                           &m_data->logClusters[run.textStart],
	                           &m_data->visAttrs[run.glyphStart],
	                           &m_data->advanceWidths[run.glyphStart],
	                           &m_data->items[run.item].a,
	                           &x ) );
	return x;
}

std::pair<int,int> UniscribeTextBlock::RunCPtoXRange( const UniscribeTextRun& run, size_t start, size_t end ) const
{
	size_t overlapStart = (std::max)( start, run.textStart );
	size_t overlapEnd   = (std::min)( end,   run.textStart + run.textCount );

	if ( overlapStart >= overlapEnd )
		return std::make_pair( 0, 0 );

	int xStart = RunCPtoX( run, overlapStart - run.textStart,   false );
	int xEnd   = RunCPtoX( run, overlapEnd - run.textStart - 1, true  );

	if ( xEnd < xStart )
		std::swap( xStart, xEnd );

	return std::make_pair( xStart, xEnd );
}

int UniscribeTextBlock::CPtoX( size_t line, size_t cp, bool trailingEdge ) const
{
	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );

	int xStart;
	const UniscribeTextRun* run = RunContaining( runs, cp, &xStart );
	assert( run != runs.end() );

	return xStart + RunCPtoX( *run, cp - run->textStart, trailingEdge );
}

size_t UniscribeTextBlock::XtoCP( size_t line, LONG* x ) const
{
	ArrayRef<const UniscribeTextRun> runs = LineRuns( line );
	assert( !runs.empty() );

	int xStart;
	const UniscribeTextRun* run = RunContaining( runs, *x, &xStart );

	if ( run == runs.end() )
	{
		size_t textEnd = TextEnd( line );
		*x = CPtoX( line, textEnd - 1, true );
		return textEnd;
	}

	int cp;
	int trailing;
	ThrowHRESULT( ScriptXtoCP( *x - xStart,
	                           run->textCount,
	                           run->glyphCount,
	                           &m_data->logClusters[run->textStart],
	                           &m_data->visAttrs[run->glyphStart],
	                           &m_data->advanceWidths[run->glyphStart],
	                           &m_data->items[run->item].a,
	                           &cp,
	                           &trailing ) );

	*x = xStart + RunCPtoX( *run, cp, trailing ? true : false );
	return run->textStart + cp + trailing;
}

std::vector<int> UniscribeTextBlock::VisualToLogicalMapping( ArrayRef<const UniscribeTextRun> runs ) const
{
	std::vector<int> visualToLogical( runs.size() );

	if ( runs.empty() )
		return visualToLogical;

	std::vector<BYTE> bidiLevel( runs.size() );

	for ( size_t i = 0; i < runs.size(); ++i )
		bidiLevel[i] = m_data->items[runs[i].item].a.s.uBidiLevel;

	ThrowHRESULT( ScriptLayout( runs.size(), &bidiLevel.front(), &visualToLogical.front(), NULL ) );
	return visualToLogical;
}

size_t UniscribeTextBlock::LineContaining( size_t pos ) const
{
	size_t line = 0;
	while ( line < m_data->lines.size() && TextEnd( line ) <= pos )
		++line;
	return line;
}

const UniscribeTextRun* UniscribeTextBlock::RunContaining( ArrayRef<const UniscribeTextRun> runs, size_t pos, int* xStart ) const
{
	const UniscribeTextRun* result = runs.begin();
	while ( result < runs.end() && result->textStart + result->textCount <= pos )
		++result;

	*xStart = 0;
	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeTextRun* run = &runs[visualToLogical[i]];

		if ( run == result )
			break;

		*xStart += run->width;
	}

	return result;
}

const UniscribeTextRun* UniscribeTextBlock::RunContaining( ArrayRef<const UniscribeTextRun> runs, int x, int* xStart ) const
{
	*xStart = 0;

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeTextRun* run = &runs[visualToLogical[i]];

		if ( x < *xStart + run->width )
			return run;

		*xStart += run->width;
	}

	return runs.end();
}
