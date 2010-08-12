// UniscribeTextBlock.cpp

#include "UniscribeTextBlock.h"
#include "UniscribeAllocator.h"
#include "VisualPainter.h"
#include "Assert.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

#undef min
#undef max

UniscribeTextBlock::UniscribeTextBlock( UniscribeAllocator& allocator, TextStyle& style, bool endsWithNewline )
	: m_length( 0 )
	, m_style( style )
	, m_lines( allocator.lines.Finish() )
	, m_runs( allocator.runs.Finish() )
	, m_items( allocator.items.Finish() )
	, m_logClusters( allocator.logClusters.Finish() )
	, m_glyphs( allocator.glyphs.Finish() )
	, m_visAttrs( allocator.visAttrs.Finish() )
	, m_advanceWidths( allocator.advanceWidths.Finish() )
	, m_offsets( allocator.offsets.Finish() )
{
	Assert( !m_lines.empty() );
	Assert( !m_runs.empty() );
	Assert( m_lines.back() == m_runs.size() );

	m_length = m_runs.back().textStart + m_runs.back().textCount;

	if ( endsWithNewline )
		m_length++;
}

void UniscribeTextBlock::Draw( VisualPainter& painter, RECT rect ) const
{
	size_t line = rect.top / m_style.lineHeight;
	rect.top = line * m_style.lineHeight;

	for ( ; line < m_lines.size() && !IsRectEmpty( &rect ); ++line )
	{
		DrawLineSelection( line, painter, rect );
		DrawLineText     ( line, painter, rect );

		rect.top += m_style.lineHeight;
	}
}

void UniscribeTextBlock::DrawLineSelection( size_t line, VisualPainter& painter, RECT rect ) const
{
	VisualSelection selection;

	if ( painter.selection.Intersects( TextStart( line ), TextEnd( line ) ) )
		selection = MakeVisualSelection( line, painter.selection );

	if ( &line == &m_lines.back() && painter.selection.Intersects( TextEnd( line ), m_length ) && EndsWithNewline() )
		selection.Add( LineWidth( line ), LineWidth( line ) + painter.style.avgCharWidth );

	selection.Draw( painter, rect );
}

void UniscribeTextBlock::DrawLineText( size_t line, VisualPainter& painter, RECT rect ) const
{
	ArrayOf<const UniscribeRun> runs = LineRuns( line );

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size() && !IsRectEmpty( &rect ); ++i )
	{
		const UniscribeRun* run = runs.begin() + visualToLogical[i];

		painter.SetFont( run->font );
		W::ThrowHRESULT( ScriptTextOut( painter.hdc,
		                                &painter.style.fonts[run->font].fontCache,
		                                rect.left,
		                                rect.top,
		                                0,
		                                &rect,
		                                &m_items[run->item].a,
		                                0,
		                                0,
		                                &m_glyphs.front() + run->glyphStart,
		                                run->glyphCount,
		                                &m_advanceWidths.front() + run->glyphStart,
		                                NULL,
		                                &m_offsets.front() + run->glyphStart ) );
		rect.left += run->width;
	}
}

VisualSelection UniscribeTextBlock::MakeVisualSelection( size_t line, TextSelection selection ) const
{
	ArrayOf<const UniscribeRun> runs = LineRuns( line );

	VisualSelection visualSelection;

	int    xRunStart = 0;
	size_t selStart  = selection.Min();
	size_t selEnd    = selection.Max();

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeRun* run = runs.begin() + visualToLogical[i];

		size_t overlapStart = std::max( selStart, run->textStart );
		size_t overlapEnd   = std::min( selEnd,   run->textStart + run->textCount );

		if ( overlapStart < overlapEnd )
		{
			int xStart = xRunStart + RunCPtoX( run, overlapStart - run->textStart,   false );
			int xEnd   = xRunStart + RunCPtoX( run, overlapEnd - 1 - run->textStart, true  );

			if ( xEnd < xStart )
				std::swap( xStart, xEnd );

			visualSelection.Add( xStart, xEnd );
		}

		xRunStart += run->width;
	}

	return visualSelection;
}

POINT UniscribeTextBlock::PointFromChar( size_t pos, bool advancing ) const
{
	POINT result;
	bool trailingEdge = advancing;

	if ( pos == 0 )
		trailingEdge = false;
	else if ( pos >= TextEnd( m_lines.size() - 1 ) )
		trailingEdge = true;

	if ( trailingEdge )
		--pos;

	size_t line = LineContaining( pos );
	Assert( line < m_lines.size() );

	if ( line < m_lines.size() )
	{
		result.x = CPtoX( LineRuns( line ), pos, trailingEdge );
		result.y = line * m_style.lineHeight;
	}
	else
	{
		result.x = 0;
		result.y = 0;
	}

	return result;
}

size_t UniscribeTextBlock::CharFromPoint( POINT* point ) const
{
	int line = point->y / m_style.lineHeight;

	if ( line < 0 || size_t( line ) >= m_lines.size() )
		line = m_lines.size() - 1;

	point->y = line * m_style.lineHeight;
	return XtoCP( LineRuns( line ), &point->x );
}

size_t UniscribeTextBlock::LineStart( int y ) const
{
	int line = y / m_style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_lines.size() );
	return TextStart( line );
}

size_t UniscribeTextBlock::LineEnd( int y ) const
{
	int line = y / m_style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_lines.size() );
	return TextEnd( line );
}

size_t UniscribeTextBlock::LineCount() const
{
	return m_lines.size();
}

size_t UniscribeTextBlock::Length() const
{
	return m_length;
}

int UniscribeTextBlock::Height() const
{
	return m_lines.size() * m_style.lineHeight;
}

bool UniscribeTextBlock::EndsWithNewline() const
{
	return m_length > m_runs.back().textStart + m_runs.back().textCount;
}

ArrayOf<const UniscribeRun> UniscribeTextBlock::LineRuns( size_t line ) const
{
	Assert( line < m_lines.size() );

	const UniscribeRun* runStart = &m_runs.front() + ( line > 0 ? m_lines[line - 1] : 0 );
	return ArrayOf<const UniscribeRun>( runStart, runStart + m_lines[line] );
}

size_t UniscribeTextBlock::TextStart( size_t line ) const
{
	ArrayOf<const UniscribeRun> runs = LineRuns( line );
	Assert( !runs.empty() );
	return runs[0].textStart;
}

size_t UniscribeTextBlock::TextEnd( size_t line ) const
{
	ArrayOf<const UniscribeRun> runs = LineRuns( line );
	Assert( !runs.empty() );
	return runs[runs.size() - 1].textStart + runs[runs.size() - 1].textCount;
}

int UniscribeTextBlock::LineWidth( size_t line ) const
{
	ArrayOf<const UniscribeRun> runs = LineRuns( line );

	int width = 0;
	for ( const UniscribeRun* run = runs.begin(); run != runs.end(); ++run )
		width += run->width;
	return width;
}

int UniscribeTextBlock::RunCPtoX( const UniscribeRun* run, size_t cp, bool trailingEdge ) const
{
	int x;
	W::ThrowHRESULT( ScriptCPtoX( cp,
	                              trailingEdge,
	                              run->textCount,
	                              run->glyphCount,
	                              &m_logClusters.front()   + run->textStart,
	                              &m_visAttrs.front()      + run->glyphStart,
	                              &m_advanceWidths.front() + run->glyphStart,
	                              &m_items[run->item].a,
	                              &x ) );
	return x;
}

int UniscribeTextBlock::CPtoX( ArrayOf<const UniscribeRun> runs, size_t cp, bool trailingEdge ) const
{
	int xStart;
	const UniscribeRun* run = RunContaining( runs, cp, &xStart );
	Assert( run != runs.end() );

	return xStart + RunCPtoX( run, cp - run->textStart, trailingEdge );
}

size_t UniscribeTextBlock::XtoCP( ArrayOf<const UniscribeRun> runs, LONG* x ) const
{
	Assert( !runs.empty() );

	int xStart;
	const UniscribeRun* run = RunContaining( runs, *x, &xStart );

	if ( run == runs.end() )
	{
		size_t textEnd = runs[runs.size() - 1].textStart + runs[runs.size() - 1].textCount;
		*x = CPtoX( runs, textEnd - 1, true );
		return textEnd;
	}

	int cp;
	int trailing;
	W::ThrowHRESULT( ScriptXtoCP( *x - xStart,
	                              run->textCount,
	                              run->glyphCount,
	                              &m_logClusters.front()   + run->textStart,
	                              &m_visAttrs.front()      + run->glyphStart,
	                              &m_advanceWidths.front() + run->glyphStart,
	                              &m_items[run->item].a,
	                              &cp,
	                              &trailing ) );

	*x = xStart + RunCPtoX( run, cp, trailing ? true : false );
	return run->textStart + cp + trailing;
}

std::vector<int> UniscribeTextBlock::VisualToLogicalMapping( ArrayOf<const UniscribeRun> runs ) const
{
	std::vector<int> visualToLogical( runs.size() );

	if ( runs.empty() )
		return visualToLogical;

	std::vector<BYTE> bidiLevel( runs.size() );

	for ( size_t i = 0; i < runs.size(); ++i )
		bidiLevel[i] = m_items[runs[i].item].a.s.uBidiLevel;

	W::ThrowHRESULT( ScriptLayout( runs.size(), &bidiLevel.front(), &visualToLogical.front(), NULL ) );
	return visualToLogical;
}

size_t UniscribeTextBlock::LineContaining( size_t pos ) const
{
	size_t line = 0;
	while ( line < m_lines.size() && TextEnd( line ) <= pos )
		++line;
	return line;
}

const UniscribeRun* UniscribeTextBlock::RunContaining( ArrayOf<const UniscribeRun> runs, size_t pos, int* xStart ) const
{
	const UniscribeRun* result = runs.begin();
	while ( result < runs.end() && result->textStart + result->textCount <= pos )
		++result;

	*xStart = 0;
	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeRun* run = runs.begin() + visualToLogical[i];

		if ( run == result )
			break;

		*xStart += run->width;
	}

	return result;
}

const UniscribeRun* UniscribeTextBlock::RunContaining( ArrayOf<const UniscribeRun> runs, int x, int* xStart ) const
{
	*xStart = 0;

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeRun* run = runs.begin() + visualToLogical[i];

		if ( x < *xStart + run->width )
			return run;

		*xStart += run->width;
	}

	return runs.end();
}
