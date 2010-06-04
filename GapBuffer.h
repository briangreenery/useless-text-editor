// GapBuffer.h

#ifndef GapBuffer_h
#define GapBuffer_h

#include "SizedAutoArray.h"
#include "Assert.h"
#include <algorithm>

#undef min
#undef max

//#pragma warning ( disable : 4996 )

/* A gap buffer is a resizing array where clustered insertions, deletions,
 * and reads are fast (it's on wikipedia). This implementation can only
 * hold POD types.
 */

template < class Unit >
class GapBuffer
{
public:
	GapBuffer();

	void insert( size_t pos, Unit unit );
	void insert( size_t pos, const Unit* units, size_t count );

	void erase( size_t pos, size_t count );
	void clear();

	size_t count( size_t pos, size_t num, Unit unit ) const;

	size_t copy( Unit* buf, size_t num, size_t pos ) const;

	      Unit& operator[] ( size_t pos )       { return At( pos ); }
	const Unit& operator[] ( size_t pos ) const { return At( pos ); }

	bool   empty() const                        { return m_size == 0; }
	size_t size() const                         { return m_size; }
	size_t capacity() const                     { return m_buffer.size(); }

private:
	size_t GapLength() const                    { return m_buffer.size() - m_size; }

	void MoveGapTo( size_t pos );
	void GrowByAtLeast( size_t amount );

	Unit& At( size_t pos ) const;

	size_t m_size;
	size_t m_gapPos;
	SizedAutoArray<Unit> m_buffer;
};

template < class Unit >
GapBuffer<Unit>::GapBuffer()
	: m_size( 0 )
	, m_gapPos( 0 )
{
}

template < class  Unit >
void GapBuffer<Unit>::MoveGapTo( size_t pos )
{
	if ( m_gapPos != pos )
	{
		Assert( m_buffer.size() >= pos + GapLength() );

		if ( pos > m_gapPos )
		{
			std::copy( m_buffer.begin() + m_gapPos + GapLength(),
			           m_buffer.begin() + pos      + GapLength(),
			           m_buffer.begin() + m_gapPos );
		}
		else
		{
			std::copy_backward( m_buffer.begin() + pos,
			                    m_buffer.begin() + m_gapPos,
			                    m_buffer.begin() + m_gapPos + GapLength() );
		}

		m_gapPos = pos;
	}
}

template < class Unit >
void GapBuffer<Unit>::GrowByAtLeast( size_t amount )
{
	size_t newCapacity = m_buffer.size() + std::max( amount, m_buffer.size() );
	SizedAutoArray<Unit> newBuffer = CreateArray<Unit>( newCapacity );

	copy( newBuffer, m_size, 0 );

	m_buffer = newBuffer;
	m_gapPos = m_size;
}

template < class Unit >
void GapBuffer<Unit>::insert( size_t pos, Unit unit )
{
	insert( pos, &unit, 1 );
}

template < class Unit >
void GapBuffer<Unit>::insert( size_t pos, const Unit* units, size_t count )
{
	if ( GapLength() < count )
		GrowByAtLeast( count - GapLength() );

	MoveGapTo( pos );
	std::copy( units, units + count, m_buffer + m_gapPos );

	m_gapPos += count;
	m_size   += count;
}

template < class Unit >
void GapBuffer<Unit>::erase( size_t pos, size_t count )
{
	Assert( pos + count <= m_size );
	MoveGapTo( pos );
	m_size -= count;
}

template < class Unit >
void GapBuffer<Unit>::clear()
{
	m_size = m_gapPos = 0;
	m_buffer.Reset( 0 );
}

template < class Unit >
size_t GapBuffer<Unit>::count( size_t pos, size_t num, Unit unit ) const
{
	Assert( pos + num <= m_size );

	size_t unitCount  = 0;
	size_t numScanned = 0;

	if ( pos < m_gapPos )
	{
		size_t numToScan = std::min( num, m_gapPos - pos );

		const Unit* start = m_buffer.begin() + pos;
		unitCount  += std::count( start, start + numToScan, unit );
		numScanned += numToScan;
	}

	if ( numScanned < num )
	{
		size_t numToScan = std::min( m_size - m_gapPos, num - numScanned );

		const Unit* start = m_buffer.begin() + pos + numScanned + GapLength();
		unitCount += std::count( start, start + numToScan, unit );
	}

	return unitCount;
}

template < class Unit >
size_t GapBuffer<Unit>::copy( Unit* buffer, size_t num, size_t pos ) const
{
	if ( pos >= m_size )
		return 0;

	size_t numCopied = 0;

	if ( pos < m_gapPos )
	{
		size_t numToCopy = std::min( num, m_gapPos - pos );

		const Unit* start = m_buffer.begin() + pos;
		std::copy( start, start + numToCopy, buffer );
		numCopied += numToCopy;
	}

	if ( numCopied < num )
	{
		size_t numToCopy = std::min( m_size - m_gapPos, num - numCopied );

		const Unit* start = m_buffer.begin() + pos + numCopied + GapLength();
		std::copy( start, start + numToCopy, buffer + numCopied );
		numCopied += numToCopy;
	}

	return numCopied;
}

template < class Unit >
Unit& GapBuffer<Unit>::At( size_t pos ) const
{
	Assert( pos <= m_size );

	if ( pos >= m_gapPos )
		pos += GapLength();

	return m_buffer[pos];
}

#endif
