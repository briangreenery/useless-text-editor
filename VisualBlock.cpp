// VisualBlock.cpp

#include "VisualBlock.h"
#include "VisualPainter.h"
#include "Assert.h"
#include <algorithm>

#undef min
#undef max

VisualBlock::VisualBlock( size_t length )
	: m_length( length )
{
	m_lines.push_back( UniscribeLine() );
}

VisualBlock::VisualBlock( size_t length, std::vector<UniscribeLine>& lines )
	: m_length( length )
	, m_lines( lines )
{
}

void VisualBlock::Draw( VisualPainter& painter, RECT rect ) const
{
	size_t i = rect.top / painter.style.lineHeight;
	rect.top = i * painter.style.lineHeight;

	for ( ; i < m_lines.size() && !IsRectEmpty( &rect ); ++i )
	{
		DrawSelection( m_lines[i], painter, rect );
		m_lines[i].Draw( painter, rect );

		rect.top += painter.style.lineHeight;
	}
}

void VisualBlock::DrawSelection( const UniscribeLine& line, VisualPainter& painter, RECT rect ) const
{
	VisualSelection selection;

	if ( painter.selection.Intersects( line.TextStart(), line.TextEnd() ) )
		selection = line.MakeVisualSelection( painter.selection );

	if ( &line == &m_lines.back() && painter.selection.Intersects( line.TextEnd(), m_length ) && EndsWithNewline() )
		selection.Add( line.Width(), line.Width() + painter.style.avgCharWidth );

	selection.Draw( painter, rect );
}

POINT VisualBlock::PointFromChar( size_t pos, bool advancing, TextStyle& style ) const
{
	POINT result;
	bool trailingEdge = advancing;

	if ( pos == 0 )
		trailingEdge = false;
	else if ( pos >= m_lines.back().TextEnd() )
		trailingEdge = true;

	if ( trailingEdge )
		--pos;

	size_t line = LineContaining( pos );

 	if ( line < m_lines.size() )
	{
		result.x = m_lines[line].CPtoX( pos, trailingEdge );
		result.y = line * style.lineHeight;
	}
	else
	{
		result.x = 0;
		result.y = 0;
	}

	return result;
}

size_t VisualBlock::CharFromPoint( POINT* point, TextStyle& style ) const
{
	int line = point->y / style.lineHeight;

	if ( line < 0 || size_t( line ) >= m_lines.size() )
		line = m_lines.size() - 1;

	point->y = line * style.lineHeight;
	return m_lines[line].XtoCP( &point->x );
}

size_t VisualBlock::LineStart( int y, TextStyle& style ) const
{
	int line = y / style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_lines.size() );
	return m_lines[line].TextStart();
}

size_t VisualBlock::LineEnd( int y, TextStyle& style ) const
{
	int line = y / style.lineHeight;
	Assert( line >= 0 && size_t( line ) < m_lines.size() );
	return m_lines[line].TextEnd();
}

size_t VisualBlock::LineCount() const
{
	return m_lines.size();
}

struct CompareLines
{
	bool operator()( const UniscribeLine& line, size_t pos ) const
	{
		return line.TextEnd() <= pos;
	}

	bool operator()( size_t pos, const UniscribeLine& line ) const
	{
		return pos < line.TextStart();
	}

	bool operator()( const UniscribeLine& a, const UniscribeLine& b ) const
	{
		return a.TextStart() < b.TextStart();
	}
};

size_t VisualBlock::LineContaining( size_t pos ) const
{
	return std::lower_bound( m_lines.begin(), m_lines.end(), pos, CompareLines() ) - m_lines.begin();
}

size_t VisualBlock::Length() const
{
	return m_length;
}

int VisualBlock::Height( TextStyle& style ) const
{
	return m_lines.size() * style.lineHeight;
}

bool VisualBlock::EndsWithNewline() const
{
	return m_lines.back().TextEnd() < m_length;
}
