// SelectionLineLoop.cpp

#include "SelectionLineLoop.h"
#include "Assert.h"
#include <limits>

#undef min
#undef max

// This computes the symmetric difference of the lines in 'top' and 'bottom'.
// I feel like there's a simpler algorithm, but I don't know what it is.

SelectionLineLoop::SelectionLineLoop( const SelectionRanges& top, const SelectionRanges& bottom )
	: m_top( top )
	, m_bottom( bottom )
	, m_t( top.begin() )
	, m_b( bottom.begin() )
	, m_topUsed( 0 )
	, m_bottomUsed( 0 )
{
	NextDifference();

	if ( Unfinished() )
		Set();
}

void SelectionLineLoop::operator++()
{
	Assert( Unfinished() );
	Assert( TopStart() != BottomStart() );

	if ( TopStart() < BottomStart() )
		AdvanceTop( m_end - m_start );
	else
		AdvanceBottom( m_end - m_start );

	NextDifference();

	if ( Unfinished() )
		Set();
}

void SelectionLineLoop::Set()
{
	Assert( Unfinished() );
	Assert( TopStart() != BottomStart() );

	if ( TopStart() < BottomStart() )
	{
		m_start = TopStart();
		m_end   = (std::min)( TopEnd(), BottomStart() );
		m_isTop = true;
	}
	else
	{
		m_start = BottomStart();
		m_end   = (std::min)( BottomEnd(), TopStart() );
		m_isTop = false;
	}
}

void SelectionLineLoop::NextDifference()
{
	while ( m_t != m_top.end() && m_b != m_bottom.end() )
	{
		if ( TopStart() != BottomStart() )
			break;

		int used = (std::min)( TopEnd(), BottomEnd() ) - TopStart();
		AdvanceTop( used );
		AdvanceBottom( used );
	}
}

int SelectionLineLoop::TopStart() const
{
	return ( m_t == m_top.end() )
	          ? std::numeric_limits<int>::max()
	          : m_t->first + m_topUsed;
}

int SelectionLineLoop::TopEnd() const
{
	return ( m_t == m_top.end() )
	          ? std::numeric_limits<int>::max()
	          : m_t->second;
}

int SelectionLineLoop::BottomStart() const
{
	return ( m_b == m_bottom.end() )
	          ? std::numeric_limits<int>::max()
	          : m_b->first + m_bottomUsed;
}

int SelectionLineLoop::BottomEnd() const
{
	return ( m_b == m_bottom.end() )
	          ? std::numeric_limits<int>::max()
	          : m_b->second;
}

void SelectionLineLoop::AdvanceTop( int amount )
{
	Assert( m_t != m_top.end() );

	m_topUsed += amount;

	if ( m_t->first + m_topUsed == m_t->second )
	{
		++m_t;
		m_topUsed = 0;
	}
}

void SelectionLineLoop::AdvanceBottom( int amount )
{
	Assert( m_b != m_bottom.end() );

	m_bottomUsed += amount;

	if ( m_b->first + m_bottomUsed == m_b->second )
	{
		++m_b;
		m_bottomUsed = 0;
	}
}
