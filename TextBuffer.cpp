// TextBuffer.cpp

#include "TextBuffer.h"

TextBuffer::TextBuffer()
	: m_size( 0 )
	, m_gapPos( 0 )
{
}

void TextBuffer::MoveGapTo( size_t pos )
{
	if ( m_gapPos != pos )
	{
		assert( m_buffer.size() >= pos + GapLength() );

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

void TextBuffer::GrowByAtLeast( size_t amount )
{
	size_t newCapacity = m_buffer.size() + (std::max)( amount, m_buffer.size() );
	std::vector<UTF16::Unit> newBuffer( newCapacity );

	copy( &newBuffer.front(), m_size, 0 );

	m_buffer.swap( newBuffer );
	m_gapPos = m_size;
}

void TextBuffer::insert( size_t pos, UTF16::Unit unit )
{
	insert( pos, &unit, 1 );
}

void TextBuffer::insert( size_t pos, const UTF16::Unit* units, size_t count )
{
	if ( GapLength() < count )
		GrowByAtLeast( count - GapLength() );

	MoveGapTo( pos );
	std::copy( units, units + count, m_buffer.begin() + m_gapPos );

	m_gapPos += count;
	m_size   += count;
}

void TextBuffer::erase( size_t pos, size_t count )
{
	assert( pos + count <= m_size );
	MoveGapTo( pos );
	m_size -= count;
}

size_t TextBuffer::count( size_t pos, size_t num, UTF16::Unit unit ) const
{
	assert( pos + num <= m_size );

	size_t unitCount  = 0;
	size_t numScanned = 0;

	if ( pos < m_gapPos )
	{
		size_t numToScan = (std::min)( num, m_gapPos - pos );

		const UTF16::Unit* start = &m_buffer.front() + pos;
		unitCount  += std::count( start, start + numToScan, unit );
		numScanned += numToScan;
	}

	if ( numScanned < num )
	{
		size_t numToScan = (std::min)( m_size - m_gapPos, num - numScanned );

		const UTF16::Unit* start = &m_buffer.front() + pos + numScanned + GapLength();
		unitCount += std::count( start, start + numToScan, unit );
	}

	return unitCount;
}

size_t TextBuffer::copy( UTF16::Unit* buffer, size_t num, size_t pos ) const
{
	if ( pos >= m_size )
		return 0;

	size_t numCopied = 0;

	if ( pos < m_gapPos )
	{
		size_t numToCopy = (std::min)( num, m_gapPos - pos );

		const UTF16::Unit* start = &m_buffer.front() + pos;
		std::copy( start, start + numToCopy, buffer );
		numCopied += numToCopy;
	}

	if ( numCopied < num )
	{
		size_t numToCopy = (std::min)( m_size - ( pos + numCopied ), num - numCopied );

		const UTF16::Unit* start = &m_buffer.front() + pos + numCopied + GapLength();
		std::copy( start, start + numToCopy, buffer + numCopied );
		numCopied += numToCopy;
	}

	return numCopied;
}
