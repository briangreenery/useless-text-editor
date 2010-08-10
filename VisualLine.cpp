// VisualLine.cpp

#include "VisualLine.h"
#include "VisualPainter.h"
#include "VisualSelection.h"
#include "TextSelection.h"
#include "Assert.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

#undef min
#undef max

VisualLine::VisualLine()
	: m_width( 0 )
	, m_textStart( 0 )
	, m_textEnd( 0 )
{
}

VisualLine::VisualLine( size_t textStart, TextRun* runStart, TextRun* runEnd )
	: m_runs( runStart, runEnd )
	, m_width( 0 )
	, m_textStart( textStart )
	, m_textEnd( textStart )
{
	for ( TextRun* run = runStart; run != runEnd; ++run )
	{
		m_width   += run->width;
		m_textEnd += run->textCount;
	}
}

void VisualLine::Draw( VisualPainter& painter, RECT rect, const LayoutData& layout ) const
{
	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping( layout );

	for ( size_t i = 0; i < visualToLogical.size() && !IsRectEmpty( &rect ); ++i )
	{
		TextRun* run = m_runs.begin() + visualToLogical[i];

		painter.SetStyle( run->style );
		W::ThrowHRESULT( ScriptTextOut( painter.hdc,
		                                &painter.style.fonts[run->style].fontCache,
		                                rect.left,
		                                rect.top,
		                                0,
		                                &rect,
		                                &layout.items[run->item].a,
		                                0,
		                                0,
		                                layout.glyphs.begin() + run->glyphStart,
		                                run->glyphCount,
		                                layout.advanceWidths.begin() + run->glyphStart,
		                                NULL,
		                                layout.offsets.begin() + run->glyphStart ) );
		rect.left += run->width;
	}
}

VisualSelection VisualLine::MakeVisualSelection( TextSelection selection, const LayoutData& layout ) const
{
	VisualSelection visualSelection;

	int    xRunStart = 0;
	size_t selStart  = selection.Min();
	size_t selEnd    = selection.Max();

	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping( layout );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		TextRun* run = m_runs.begin() + visualToLogical[i];

		size_t overlapStart = std::max( selStart, run->textStart );
		size_t overlapEnd   = std::min( selEnd,   run->textStart + run->textCount );

		if ( overlapStart < overlapEnd )
		{
			int xStart = xRunStart + RunCPtoX( run, overlapStart - run->textStart,   false, layout );
			int xEnd   = xRunStart + RunCPtoX( run, overlapEnd - 1 - run->textStart, true,  layout );

			if ( xEnd < xStart )
				std::swap( xStart, xEnd );

			visualSelection.Add( xStart, xEnd );
		}

		xRunStart += run->width;
	}

	return visualSelection;
}

LONG VisualLine::RunCPtoX( TextRun* run, size_t cp, bool trailingEdge, const LayoutData& layout ) const
{
	int x;
	W::ThrowHRESULT( ScriptCPtoX( cp,
	                              trailingEdge,
	                              run->textCount,
	                              run->glyphCount,
	                              layout.logClusters.begin()   + run->textStart,
	                              layout.visAttrs.begin()      + run->glyphStart,
	                              layout.advanceWidths.begin() + run->glyphStart,
	                              &layout.items[run->item].a,
	                              &x ) );
	return x;
}

LONG VisualLine::CPtoX( size_t pos, bool trailingEdge, const LayoutData& layout ) const
{
	int xStart;
	TextRun* run = RunContaining( pos, &xStart, layout );
	Assert( run != m_runs.end() );

	return xStart + RunCPtoX( run, pos - run->textStart, trailingEdge, layout );
}

size_t VisualLine::XtoCP( LONG* x, const LayoutData& layout ) const
{
	if ( m_runs.empty() )
	{
		*x = 0;
		return TextStart();
	}

	int xStart;
	TextRun* run = RunContaining( *x, &xStart, layout );

	if ( run == m_runs.end() )
	{
		*x = CPtoX( TextEnd() - 1, true, layout );
		return TextEnd();
	}

	int cp;
	int trailing;
	W::ThrowHRESULT( ScriptXtoCP( *x - xStart,
	                              run->textCount,
	                              run->glyphCount,
	                              layout.logClusters.begin()   + run->textStart,
	                              layout.visAttrs.begin()      + run->glyphStart,
	                              layout.advanceWidths.begin() + run->glyphStart,
	                              &layout.items[run->item].a,
	                              &cp,
	                              &trailing ) );

	*x = xStart + RunCPtoX( run, cp, trailing ? true : false, layout );
	return run->textStart + cp + trailing;
}

struct CompareRuns
{
	bool operator()( const TextRun& run, size_t pos ) const
	{
		return run.textStart + run.textCount <= pos;
	}

	bool operator()( size_t pos, const TextRun& run ) const
	{
		return pos < run.textStart;
	}

	bool operator()( const TextRun& a, const TextRun& b ) const
	{
		return a.textStart < b.textStart;
	}
};

TextRun* VisualLine::RunContaining( size_t pos, int* xStart, const LayoutData& layout ) const
{
	TextRun* result = std::lower_bound( m_runs.begin(), m_runs.end(), pos, CompareRuns() );

	*xStart = 0;
	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping( layout );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		TextRun* run = m_runs.begin() + visualToLogical[i];

		if ( run == result )
			break;

		*xStart += run->width;
	}

	return result;
}

TextRun* VisualLine::RunContaining( int x, int* xStart, const LayoutData& layout ) const
{
	*xStart = 0;

	if ( x <= 0 )
		return m_runs.begin();

	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping( layout );

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		TextRun* run = m_runs.begin() + visualToLogical[i];

		if ( x < *xStart + run->width )
			return run;

		*xStart += run->width;
	}

	return m_runs.end();
}

SizedAutoArray<int> VisualLine::VisualToLogicalMapping( const LayoutData& layout ) const
{
	if ( m_runs.empty() )
		return SizedAutoArray<int>();

	SizedAutoArray<BYTE> bidiLevel = CreateArray<BYTE>( m_runs.size() );

	for ( size_t i = 0; i < m_runs.size(); ++i )
		bidiLevel[i] = layout.items[m_runs[i].item].a.s.uBidiLevel;

	SizedAutoArray<int> visualToLogical = CreateArray<int>( m_runs.size() );

	W::ThrowHRESULT( ScriptLayout( m_runs.size(),
	                               bidiLevel.begin(),
	                               visualToLogical.begin(),
	                               NULL ) );

	return visualToLogical;
}
