// LineBuffer.cpp

#include "LineBuffer.h"
#include "CharBuffer.h"
#include "CharChange.h"

LineBuffer::LineBuffer( const CharBuffer& charBuffer )
	: m_charBuffer( charBuffer )
{
	m_lineLengths.Insert( 0, 0 );
}

void LineBuffer::Update( CharChange change )
{
	size_t start = LineContaining( change.start );
	size_t end   = start;

	size_t textStart  = LineStart( start );
	size_t textLength = 0;

	do
	{
		assert( end < LineCount() );
		textLength += m_lineLengths[end++];
	}
	while ( textStart + textLength < change.end );

	m_lineLengths.Erase( start, end - start );
}

size_t LineBuffer::LineCount() const
{
	return m_lineLengths.Length();
}

size_t LineBuffer::LineStart( size_t lineIndex ) const
{
	assert( lineIndex <= LineCount() );

	size_t lineStart = 0;

	for ( size_t i = 0; i < lineIndex; ++i )
		lineStart += m_lineLengths[i];

	return lineStart;
}

size_t LineBuffer::LineLength( size_t lineIndex ) const
{
	assert( lineIndex < LineCount() );

	return m_lineLengths[lineIndex];
}

size_t LineBuffer::LineContaining( size_t charPos ) const
{
	size_t lineStart = 0;

	for ( size_t i = 0, count = LineCount(); i < count; ++i )
	{
		size_t nextLineStart = lineStart + m_lineLengths[i];

		if ( charPos < nextLineStart )
			return i;
	}

	return LineCount() - 1;
}
