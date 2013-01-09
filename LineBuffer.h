// LineBuffer.h

#ifndef LineBuffer_h
#define LineBuffer_h

#include "GapArray.h"

class CharBuffer;
class CharChange;

// LineBuffer stores where the line breaks are.

class LineBuffer
{
public:
	LineBuffer( const CharBuffer& );

	void Update( CharChange change );

	size_t LineCount() const;

	size_t LineStart ( size_t lineIndex ) const;
	size_t LineLength( size_t lineIndex ) const;

	size_t LineContaining( size_t charPos ) const;

private:
	GapArray<size_t> m_lineLengths;
	const CharBuffer& m_charBuffer;
};

#endif
