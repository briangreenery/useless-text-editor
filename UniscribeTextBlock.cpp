// UniscribeTextBlock.cpp

#include "UniscribeTextBlock.h"
#include "VisualPainter.h"
#include "VisualSelection.h"
#include "TextStyle.h"
#include "Assert.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

#undef min
#undef max

UniscribeTextBlock::UniscribeTextBlock( UniscribeLayoutDataPtr data, TextStyle& style )
	: m_data( std::move( data ) )
	, m_style( style )
{
}

void UniscribeTextBlock::Draw( VisualPainter& painter, RECT rect ) const
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

void UniscribeTextBlock::DrawLineSelection( size_t line, VisualPainter& painter, RECT rect ) const
{
	VisualSelection selection;

	if ( painter.selection.Intersects( TextStart( line ), TextEnd( line ) ) )
		selection = MakeVisualSelection( line, painter.selection );

	if ( line == m_data->lines.size() - 1
	  && painter.selection.Intersects( TextEnd( line ), m_data->length )
	  && m_data->endsWithNewline )
	{
		selection.Add( LineWidth( line ), LineWidth( line ) + painter.style.avgCharWidth );
	}

	selection.Draw( painter, rect );
}

void UniscribeTextBlock::DrawLineText( size_t line, VisualPainter& painter, RECT rect ) const
{
	ArrayOf<const UniscribeTextRun> runs = LineRuns( line );

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size() && !IsRectEmpty( &rect ); ++i )
	{
		const UniscribeTextRun* run = runs.begin() + visualToLogical[i];

		painter.SetFont( run->font );
		W::ThrowHRESULT( ScriptTextOut( painter.hdc,
		                                &painter.style.fonts[run->font].fontCache,
		                                rect.left,
		                                rect.top,
		                                0,
		                                &rect,
		                                &m_data->items[run->item].a,
		                                0,
		                                0,
		                                &m_data->glyphs.front() + run->glyphStart,
		                                run->glyphCount,
		                                &m_data->advanceWidths.front() + run->glyphStart,
		                                NULL,
		                                &m_data->offsets.front() + run->glyphStart ) );
		rect.left += run->width;
	}
}

VisualSelection UniscribeTextBlock::MakeVisualSelection( size_t line, TextSelection selection ) const
{
	ArrayOf<const UniscribeTextRun> runs = LineRuns( line );

	VisualSelection visualSelection;

	int    xRunStart = 0;
	size_t selStart  = selection.Min();
	size_t selEnd    = selection.Max();

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeTextRun* run = runs.begin() + visualToLogical[i];

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
	else if ( pos >= TextEnd( m_data->lines.size() - 1 ) )
		trailingEdge = true;

	if ( trailingEdge )
		--pos;

	size_t line = LineContaining( pos );
	Assert( line < m_data->lines.size() );

	result.x = CPtoX( LineRuns( line ), pos, trailingEdge );
	result.y = line * m_style.lineHeight;

	return result;
}

size_t UniscribeTextBlock::CharFromPoint( POINT* point ) const
{
	int line = point->y / m_style.lineHeight;

	if ( line < 0 || size_t( line ) >= m_data->lines.size() )
		line = m_data->lines.size() - 1;

	point->y = line * m_style.lineHeight;
	return XtoCP( LineRuns( line ), &point->x );
}

size_t UniscribeTextBlock::LineStart( int y ) const
{
	int line = y / m_style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_data->lines.size() );
	return TextStart( line );
}

size_t UniscribeTextBlock::LineEnd( int y ) const
{
	int line = y / m_style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_data->lines.size() );
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
	return m_data->lines.size() * m_style.lineHeight;
}

bool UniscribeTextBlock::EndsWithNewline() const
{
	return m_data->endsWithNewline;
}

ArrayOf<const UniscribeTextRun> UniscribeTextBlock::LineRuns( size_t line ) const
{
	Assert( line < m_data->lines.size() );
	const UniscribeTextRun* runStart = &m_data->runs.front() + ( line > 0 ? m_data->lines[line - 1] : 0 );
	return ArrayOf<const UniscribeTextRun>( runStart, &m_data->runs.front() + m_data->lines[line] );
}

size_t UniscribeTextBlock::TextStart( size_t line ) const
{
	ArrayOf<const UniscribeTextRun> runs = LineRuns( line );
	Assert( !runs.empty() );
	return runs[0].textStart;
}

size_t UniscribeTextBlock::TextEnd( size_t line ) const
{
	ArrayOf<const UniscribeTextRun> runs = LineRuns( line );
	Assert( !runs.empty() );
	return runs[runs.size() - 1].textStart + runs[runs.size() - 1].textCount;
}

int UniscribeTextBlock::LineWidth( size_t line ) const
{
	ArrayOf<const UniscribeTextRun> runs = LineRuns( line );

	int width = 0;
	for ( const UniscribeTextRun* run = runs.begin(); run != runs.end(); ++run )
		width += run->width;
	return width;
}

int UniscribeTextBlock::RunCPtoX( const UniscribeTextRun* run, size_t cp, bool trailingEdge ) const
{
	int x;
	W::ThrowHRESULT( ScriptCPtoX( cp,
	                              trailingEdge,
	                              run->textCount,
	                              run->glyphCount,
	                              &m_data->logClusters.front()   + run->textStart,
	                              &m_data->visAttrs.front()      + run->glyphStart,
	                              &m_data->advanceWidths.front() + run->glyphStart,
	                              &m_data->items[run->item].a,
	                              &x ) );
	return x;
}

int UniscribeTextBlock::CPtoX( ArrayOf<const UniscribeTextRun> runs, size_t cp, bool trailingEdge ) const
{
	int xStart;
	const UniscribeTextRun* run = RunContaining( runs, cp, &xStart );
	Assert( run != runs.end() );

	return xStart + RunCPtoX( run, cp - run->textStart, trailingEdge );
}

size_t UniscribeTextBlock::XtoCP( ArrayOf<const UniscribeTextRun> runs, LONG* x ) const
{
	Assert( !runs.empty() );

	int xStart;
	const UniscribeTextRun* run = RunContaining( runs, *x, &xStart );

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
	                              &m_data->logClusters.front()   + run->textStart,
	                              &m_data->visAttrs.front()      + run->glyphStart,
	                              &m_data->advanceWidths.front() + run->glyphStart,
	                              &m_data->items[run->item].a,
	                              &cp,
	                              &trailing ) );

	*x = xStart + RunCPtoX( run, cp, trailing ? true : false );
	return run->textStart + cp + trailing;
}

std::vector<int> UniscribeTextBlock::VisualToLogicalMapping( ArrayOf<const UniscribeTextRun> runs ) const
{
	std::vector<int> visualToLogical( runs.size() );

	if ( runs.empty() )
		return visualToLogical;

	std::vector<BYTE> bidiLevel( runs.size() );

	for ( size_t i = 0; i < runs.size(); ++i )
		bidiLevel[i] = m_data->items[runs[i].item].a.s.uBidiLevel;

	W::ThrowHRESULT( ScriptLayout( runs.size(), &bidiLevel.front(), &visualToLogical.front(), NULL ) );
	return visualToLogical;
}

size_t UniscribeTextBlock::LineContaining( size_t pos ) const
{
	size_t line = 0;
	while ( line < m_data->lines.size() && TextEnd( line ) <= pos )
		++line;
	return line;
}

const UniscribeTextRun* UniscribeTextBlock::RunContaining( ArrayOf<const UniscribeTextRun> runs, size_t pos, int* xStart ) const
{
	const UniscribeTextRun* result = runs.begin();
	while ( result < runs.end() && result->textStart + result->textCount <= pos )
		++result;

	*xStart = 0;
	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeTextRun* run = runs.begin() + visualToLogical[i];

		if ( run == result )
			break;

		*xStart += run->width;
	}

	return result;
}

const UniscribeTextRun* UniscribeTextBlock::RunContaining( ArrayOf<const UniscribeTextRun> runs, int x, int* xStart ) const
{
	*xStart = 0;

	std::vector<int> visualToLogical = VisualToLogicalMapping( runs );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		const UniscribeTextRun* run = runs.begin() + visualToLogical[i];

		if ( x < *xStart + run->width )
			return run;

		*xStart += run->width;
	}

	return runs.end();
}
