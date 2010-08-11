// UniscribeLine.cpp

#include "UniscribeLine.h"
#include "VisualPainter.h"
#include "VisualSelection.h"
#include "TextSelection.h"
#include "Assert.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

#undef min
#undef max

UniscribeLine::UniscribeLine()
	: m_width( 0 )
	, m_textStart( 0 )
	, m_textEnd( 0 )
{
}

UniscribeLine::UniscribeLine( ArrayOf<UniscribeRun> runs, const UniscribeDataPtr& data )
	: m_width( 0 )
	, m_runs( runs )
	, m_data( data )
{
	Assert( !runs.empty() );

	m_textStart = m_runs[0].textStart;
	m_textEnd   = m_runs[0].textStart;

	for ( UniscribeRun* run = m_runs.begin(); run != m_runs.end(); ++run )
	{
		m_width   += run->width;
		m_textEnd += run->textCount;
	}
}

void UniscribeLine::Draw( VisualPainter& painter, RECT rect ) const
{
	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping();

	for ( size_t i = 0; i < visualToLogical.size() && !IsRectEmpty( &rect ); ++i )
	{
		UniscribeRun* run = m_runs.begin() + visualToLogical[i];

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
		                                m_data->glyphs.begin() + run->glyphStart,
		                                run->glyphCount,
		                                m_data->advanceWidths.begin() + run->glyphStart,
		                                NULL,
		                                m_data->offsets.begin() + run->glyphStart ) );
		rect.left += run->width;
	}
}

VisualSelection UniscribeLine::MakeVisualSelection( TextSelection selection ) const
{
	VisualSelection visualSelection;

	int    xRunStart = 0;
	size_t selStart  = selection.Min();
	size_t selEnd    = selection.Max();

	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping();

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		UniscribeRun* run = m_runs.begin() + visualToLogical[i];

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

int UniscribeLine::RunCPtoX( UniscribeRun* run, size_t cp, bool trailingEdge ) const
{
	int x;
	W::ThrowHRESULT( ScriptCPtoX( cp,
	                              trailingEdge,
	                              run->textCount,
	                              run->glyphCount,
	                              m_data->logClusters.begin()   + run->textStart,
	                              m_data->visAttrs.begin()      + run->glyphStart,
	                              m_data->advanceWidths.begin() + run->glyphStart,
	                              &m_data->items[run->item].a,
	                              &x ) );
	return x;
}

int UniscribeLine::CPtoX( size_t pos, bool trailingEdge ) const
{
	int xStart;
	UniscribeRun* run = RunContaining( pos, &xStart );
	Assert( run != m_runs.end() );

	return xStart + RunCPtoX( run, pos - run->textStart, trailingEdge );
}

size_t UniscribeLine::XtoCP( LONG* x ) const
{
	if ( m_runs.empty() )
	{
		*x = 0;
		return TextStart();
	}

	int xStart;
	UniscribeRun* run = RunContaining( *x, &xStart );

	if ( run == m_runs.end() )
	{
		*x = CPtoX( TextEnd() - 1, true );
		return TextEnd();
	}

	int cp;
	int trailing;
	W::ThrowHRESULT( ScriptXtoCP( *x - xStart,
	                              run->textCount,
	                              run->glyphCount,
	                              m_data->logClusters.begin()   + run->textStart,
	                              m_data->visAttrs.begin()      + run->glyphStart,
	                              m_data->advanceWidths.begin() + run->glyphStart,
	                              &m_data->items[run->item].a,
	                              &cp,
	                              &trailing ) );

	*x = xStart + RunCPtoX( run, cp, trailing ? true : false );
	return run->textStart + cp + trailing;
}

struct CompareRuns
{
	bool operator()( const UniscribeRun& run, size_t pos ) const
	{
		return run.textStart + run.textCount <= pos;
	}

	bool operator()( size_t pos, const UniscribeRun& run ) const
	{
		return pos < run.textStart;
	}

	bool operator()( const UniscribeRun& a, const UniscribeRun& b ) const
	{
		return a.textStart < b.textStart;
	}
};

UniscribeRun* UniscribeLine::RunContaining( size_t pos, int* xStart ) const
{
	UniscribeRun* result = std::lower_bound( m_runs.begin(), m_runs.end(), pos, CompareRuns() );

	*xStart = 0;
	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping();

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		UniscribeRun* run = m_runs.begin() + visualToLogical[i];

		if ( run == result )
			break;

		*xStart += run->width;
	}

	return result;
}

UniscribeRun* UniscribeLine::RunContaining( int x, int* xStart ) const
{
	*xStart = 0;

	SizedAutoArray<int> visualToLogical = VisualToLogicalMapping();

	for ( size_t i = 0; i < visualToLogical.size(); ++i )
	{
		UniscribeRun* run = m_runs.begin() + visualToLogical[i];

		if ( x < *xStart + run->width )
			return run;

		*xStart += run->width;
	}

	return m_runs.end();
}

SizedAutoArray<int> UniscribeLine::VisualToLogicalMapping() const
{
	if ( m_runs.empty() )
		return SizedAutoArray<int>();

	SizedAutoArray<BYTE> bidiLevel = CreateArray<BYTE>( m_runs.size() );

	for ( size_t i = 0; i < m_runs.size(); ++i )
		bidiLevel[i] = m_data->items[m_runs[i].item].a.s.uBidiLevel;

	SizedAutoArray<int> visualToLogical = CreateArray<int>( m_runs.size() );

	W::ThrowHRESULT( ScriptLayout( m_runs.size(),
	                               bidiLevel.begin(),
	                               visualToLogical.begin(),
	                               NULL ) );

	return visualToLogical;
}
