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

void GapArrayBase::MoveGapTo( size_t pos ) const
{
	if ( m_gapPosition != pos )
	{
		assert( m_capacity >= pos + GapLength() );

		if ( pos > m_gapPosition )
		{
			memcpy( m_buffer + m_gapPosition,
			        m_buffer + m_gapPosition + GapLength(),
			        pos - m_gapPosition );
		}
		else
		{
			memmove( m_buffer + pos + GapLength(),
			         m_buffer + pos,
			         m_gapPosition - pos );
		}

		m_gapPosition = pos;
	}
}

void GapArrayBase::Resize( size_t newCapacity )
{
	assert( newCapacity >= m_size );

	uint8_t* newBuffer = 0;
	
	if ( newCapacity != 0 )
	{
		newBuffer = new uint8_t[newCapacity];

		memcpy( newBuffer, m_buffer, m_gapPosition );

		memcpy( newBuffer + m_gapPosition + ( newCapacity - m_size ),
		        m_buffer  + m_gapPosition + ( m_capacity  - m_size ),
		        m_size - m_gapPosition );
	}

	if ( m_buffer )
		delete[] m_buffer;

	m_buffer   = newBuffer;
	m_capacity = newCapacity;
}

void GapArrayBase::InsertBytes( size_t pos, ArrayRef<const uint8_t> bytes )
{
	if ( GapLength() < bytes.size() )
		Resize( m_capacity + std::max( m_capacity, bytes.size() - GapLength() ) );

	MoveGapTo( pos );
	memcpy( m_buffer + m_gapPosition, bytes.begin(), bytes.size() );

	m_gapPosition += bytes.size();
	m_size        += bytes.size();
}

void GapArrayBase::EraseBytes( size_t pos, size_t count )
{
	assert( pos + count <= m_size );
	MoveGapTo( pos );
	m_size -= count;

	if ( m_size < m_capacity / 4 )
		Resize( 2 * m_size );
}

ArrayRef<const uint8_t> GapArrayBase::ReadBytes( size_t pos, size_t count ) const
{
	pos   = std::min( pos,   m_size );
	count = std::min( count, m_size - pos );

	if ( pos <= m_gapPosition && m_gapPosition < pos + count )
		MoveGapTo( pos + count );

	return ArrayRef<const uint8_t>( m_buffer + pos, count );
}
