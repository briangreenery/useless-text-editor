// GapArray.cpp

#include "GapArray.h"
#include <cassert>
#include <algorithm>

GapArrayBase::GapArrayBase()
	: m_size( 0 )
	, m_gapPosition( 0 )
	, m_buffer( 0 )
{
}

GapArrayBase::~GapArrayBase()
{
	if ( m_buffer )
		delete[] m_buffer;
}

void GapArrayBase::MoveGapTo( size_t pos )
{
	if ( m_gapPosition != pos )
	{
		assert( m_capacity >= pos + GapLength() );

		if ( pos > m_gapPosition )
			memcpy( m_buffer + m_gapPosition, m_buffer + m_gapPosition + GapLength(), pos - m_gapPosition );
		else
			memmove( m_buffer + pos + GapLength(), m_buffer + pos, m_gapPosition - pos );

		m_gapPosition = pos;
	}
}

void GapArrayBase::Resize( size_t newCapacity )
{
	assert( newCapacity >= m_size );

	uint8_t* newBuffer = 0;
	
	if ( newCapacity > 0 )
	{
		newBuffer = new uint8_t[newCapacity];
		ReadBytes( 0, newBuffer, m_size );
	}

	if ( m_buffer )
		delete[] m_buffer;

	m_buffer   = newBuffer;
	m_capacity = newCapacity;
}

void GapArrayBase::InsertBytes( size_t pos, const uint8_t* buffer, size_t count )
{
	if ( GapLength() < count )
		Resize( m_capacity + std::max( m_capacity, count - GapLength() ) );

	MoveGapTo( pos );
	memcpy( m_buffer + m_gapPosition, buffer, count );

	m_gapPosition += count;
	m_size        += count;
}

void GapArrayBase::EraseBytes( size_t pos, size_t count )
{
	assert( pos + count <= m_size );
	MoveGapTo( pos );
	m_size -= count;

	if ( m_size < m_capacity / 4 )
		Resize( 2 * m_size );
}

size_t GapArrayBase::ReadBytes( size_t pos, uint8_t* buffer, size_t count ) const
{
	if ( pos >= m_size )
		return 0;

	size_t numCopied = 0;

	if ( pos < m_gapPosition )
	{
		size_t numToCopy = std::min( count, m_gapPosition - pos );
		memcpy( buffer, m_buffer + pos, numToCopy );
		numCopied += numToCopy;
	}

	if ( numCopied < count )
	{
		size_t numToCopy = std::min( m_size - ( pos + numCopied ), count - numCopied );
		memcpy( buffer + numCopied, m_buffer + pos + numCopied + GapLength(), numToCopy );
		numCopied += numToCopy;
	}

	return numCopied;
}
