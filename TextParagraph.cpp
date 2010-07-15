// TextParagraph.cpp

#include "TextParagraph.h"
#include "ScriptBreakLoop.h"
#include "DrawLineSelection.h"
#include "Require.h"
#include "Assert.h"
#include <algorithm>

TextParagraph::TextParagraph()
	: m_valid( false )
	, m_width( 0 )
	, m_length( 0 )
{
}

TextParagraph::TextParagraph( const TextParagraph& other )
	: m_valid( false )
	, m_width( 0 )
	, m_length( 0 )
{
	Assert( !other.m_valid );
}

SelectionRanges TextParagraph::Draw( HDC hdc, RECT rect, SCRIPT_CACHE cache, TextSelection selection, SelectionRanges topSelection, bool isLast ) const
{
	static const int avgCharWidth = 10;

	SelectionRanges bottomSelection;

	size_t i = rect.top / LineHeight();
	rect.top = i * LineHeight();

	if ( 0 < i && i < m_lines.size() )
		topSelection = m_lines[i - 1].MakeSelectionRanges( selection.Min(), selection.Max(), m_layout );

	for ( ; i < m_lines.size(); ++i )
	{
		//if ( IsRectEmpty( &rect ) )
		//	return;

		const VisualLine& line = m_lines[i];

		bottomSelection.clear();
		if ( selection.Intersects( line.TextStart(), line.TextEnd() ) )
			bottomSelection = line.MakeSelectionRanges( selection.Min(), selection.Max(), m_layout );

		if (   ( i == m_lines.size() - 1 )
			&& LengthWithoutNewline() != Length()
			&& selection.Intersects( LengthWithoutNewline(), Length() ) )
		{
			AddSelectionRange( SelectionRange( line.Width(), line.Width() + avgCharWidth ), bottomSelection );
		}

		RECT lineRect = { rect.left, rect.top, rect.right, rect.top + LineHeight() };
		DrawLineSelection( hdc, lineRect, topSelection, bottomSelection, isLast && ( i + 1 ) == m_lines.size() );
		std::swap( topSelection, bottomSelection );

		line.Draw( hdc, rect, m_layout, cache );
		rect.top += LineHeight();
	}

	return topSelection;
}

POINT TextParagraph::PointFromChar( size_t pos, bool advancing ) const
{
	POINT result;
	bool trailingEdge = advancing;

	if ( pos == 0 )
		trailingEdge = false;
	else if ( pos >= LengthWithoutNewline() )
		trailingEdge = true;

	if ( trailingEdge )
		--pos;

	size_t line = LineContaining( pos );

 	if ( line < m_lines.size() )
	{
		result.x = m_lines[line].CPtoX( pos, trailingEdge, m_layout );
		result.y = line * LineHeight();
	}
	else
	{
		result.x = 0;
		result.y = 0;
	}

	return result;
}

size_t TextParagraph::CharFromPoint( POINT* point ) const
{
	int line = point->y / LineHeight();

	if ( line < 0 || size_t( line ) >= m_lines.size() )
		line = m_lines.size() - 1;

	point->y = line * LineHeight();
	return m_lines[line].XtoCP( &point->x, m_layout );
}

size_t TextParagraph::LineStart( LONG y ) const
{
	int line = y / LineHeight();
	Require( line >= 0 && size_t( line ) < m_lines.size() );
	return m_lines[line].TextStart();
}

size_t TextParagraph::LineEnd( LONG y ) const
{
	int line = y / LineHeight();
	Require( line >= 0 && size_t( line ) < m_lines.size() );
	return m_lines[line].TextEnd();
}

size_t TextParagraph::LengthWithoutNewline() const
{
	return m_lines.back().TextEnd();
}

struct CompareLines
{
	bool operator()( const VisualLine& line, size_t pos ) const
	{
		return line.TextEnd() <= pos;
	}

	bool operator()( size_t pos, const VisualLine& line ) const
	{
		return pos < line.TextStart();
	}

	bool operator()( const VisualLine& a, const VisualLine& b ) const
	{
		return a.TextStart() < b.TextStart();
	}
};

size_t TextParagraph::LineContaining( size_t pos ) const
{
	return std::lower_bound( m_lines.begin(), m_lines.end(), pos, CompareLines() ) - m_lines.begin();
}

void TextParagraph::Validate( LayoutAllocator& allocator )
{
	Assert( m_length > 0 );
	m_valid = true;
	m_width = 0;
	m_layout.Copy( allocator );
	m_lines.swap( VisualLines() );

	ArrayOf<size_t> lines = allocator.lines.Allocated();
	m_lines.reserve( lines.size() );

	TextRun* runs      = m_layout.runs;
	size_t   lastEnd   = 0;
	size_t   textStart = 0;

	for ( size_t* it = lines.begin(); it != lines.end(); ++it )
	{
		TextRun* runStart = runs + lastEnd;
		TextRun* runEnd   = runs + *it;

		m_lines.push_back( VisualLine( textStart, runStart, runEnd ) );

		if ( runStart != runEnd )
			textStart = runEnd[-1].textStart + runEnd[-1].textCount;

		lastEnd = *it;
		m_width = (std::max)( m_width, m_lines.back().Width() );
	}
}

void TextParagraph::Validate()
{
	Assert( m_length == 0 || m_length == 1 );
	m_valid = true;
	m_width = 0;
	m_layout.Reset();
	m_lines.swap( VisualLines() );

	m_lines.reserve( 1 );
	m_lines.push_back( VisualLine() );
}

void TextParagraph::ResetLength( size_t length )
{
	m_length = length;
	m_valid  = false;
}
