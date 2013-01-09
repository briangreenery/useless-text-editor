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
			memcpy_s( m_buffer + m_gapPosition, m_capacity - m_gapPosition, m_buffer + m_gapPosition + GapLength(), pos - m_gapPosition );
		else
			memmove_s( m_buffer + pos + GapLength(), pos + m_size, m_buffer + pos, m_gapPosition - pos );

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
		ReadBytes( 0, m_size, ArrayRef<uint8_t>( newBuffer, newCapacity ) );
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

size_t GapArrayBase::ReadBytes( size_t pos, size_t count, ArrayRef<uint8_t> output ) const
{
	size_t numCopied = 0;
	size_t numToCopy = std::min( count, output.size() );

	if ( pos < m_gapPosition )
	{
		size_t chunkSize = std::min( numToCopy, m_gapPosition - pos );
		memcpy( output.begin(), m_buffer + pos, chunkSize );
		numCopied += chunkSize;
	}

	if ( numCopied < numToCopy )
	{
		size_t chunkSize = std::min( m_size - ( pos + numCopied ), count - numCopied );
		memcpy( output.begin() + numCopied, m_buffer + pos + numCopied + GapLength(), chunkSize );
		numCopied += chunkSize;
	}

	return numCopied;
}
