// TextBuffer.h

#ifndef TextBuffer_h
#define TextBuffer_h

#include "UString.h"
#include <cassert>
#include <algorithm>
#include <vector>

class TextBuffer
{
public:
	TextBuffer();

	void insert( size_t pos, UTF16::Unit unit );
	void insert( size_t pos, const UTF16::Unit* units, size_t count );

	void erase( size_t pos, size_t count );

	size_t count( size_t pos, size_t num, UTF16::Unit unit ) const;

	size_t copy( UTF16::Unit* buf, size_t num, size_t pos ) const;

	UTF16::Unit operator[] ( size_t pos ) const;

	bool   empty() const { return m_size == 0; }
	size_t size() const  { return m_size; }

private:
	size_t GapLength() const { return m_buffer.size() - m_size; }

	void MoveGapTo( size_t pos );
	void GrowByAtLeast( size_t amount );

	size_t m_size;
	size_t m_gapPos;
	std::vector<UTF16::Unit> m_buffer;
};

inline UTF16::Unit TextBuffer::operator[]( size_t pos ) const
{
	assert( pos <= m_size );

	if ( pos >= m_gapPos )
		pos += GapLength();

	return m_buffer[pos];
}

#endif
