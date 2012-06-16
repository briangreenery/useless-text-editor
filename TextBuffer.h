// TextBuffer.h

#ifndef TextBuffer_h
#define TextBuffer_h

#include "UTF16Ref.h"
#include <cassert>
#include <algorithm>
#include <vector>

class TextBuffer
{
public:
	TextBuffer();

	void insert( size_t pos, wchar_t unit );
	void insert( size_t pos, const wchar_t* units, size_t count );

	void erase( size_t pos, size_t count );

	size_t count( size_t pos, size_t num, wchar_t unit ) const;

	size_t copy( wchar_t* buf, size_t num, size_t pos ) const;

	wchar_t operator[] ( size_t pos ) const;

	bool   empty() const { return m_size == 0; }
	size_t size() const  { return m_size; }

private:
	size_t GapLength() const { return m_buffer.size() - m_size; }

	void MoveGapTo( size_t pos );
	void GrowByAtLeast( size_t amount );

	size_t m_size;
	size_t m_gapPos;
	std::vector<wchar_t> m_buffer;
};

inline wchar_t TextBuffer::operator[]( size_t pos ) const
{
	assert( pos <= m_size );

	if ( pos >= m_gapPos )
		pos += GapLength();

	return m_buffer[pos];
}

#endif
