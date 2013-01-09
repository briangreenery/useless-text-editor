// CharBuffer.h

#ifndef CharBuffer_h
#define CharBuffer_h

#include "GapArray.h"
#include <unicode/brkiter.h>

class CharRange;
class CharChange;

class CharBuffer
{
public:
	CharBuffer();
	~CharBuffer();

	CharChange Insert( size_t pos, wchar_t );
	CharChange Insert( size_t pos, ArrayRef<const wchar_t> );
	CharChange Delete( size_t pos, size_t count );

	ArrayRef<wchar_t> Read( size_t start, size_t count, ArrayRef<wchar_t> buffer ) const;

	size_t Length() const                    { return m_buffer.Length(); }
	wchar_t operator[]( size_t pos ) const   { return m_buffer[pos]; }

	size_t NextCharStop( size_t pos ) const  { return NextBreak( m_charIter, pos ); }
	size_t PrevCharStop( size_t pos ) const  { return PrevBreak( m_charIter, pos ); }

	size_t NextWordStop( size_t pos ) const  { return NextBreak( m_wordIter, pos ); }
	size_t PrevWordStop( size_t pos ) const  { return PrevBreak( m_wordIter, pos ); }

	size_t NextSoftBreak( size_t pos ) const { return NextBreak( m_lineIter, pos ); }
	size_t PrevSoftBreak( size_t pos ) const { return PrevBreak( m_lineIter, pos ); }

	size_t NextLineBreak( size_t pos ) const;

	size_t NextNonWhitespace( size_t pos ) const;
	size_t PrevNonWhitespace( size_t pos ) const;

	CharRange WordAt( size_t pos ) const;

private:
	void ResetIterators() const;
	size_t NextBreak( icu::BreakIterator*, size_t ) const;
	size_t PrevBreak( icu::BreakIterator*, size_t ) const;

	GapArray<wchar_t> m_buffer;

	mutable bool m_needIterReset;

	icu::BreakIterator* m_charIter;
	UErrorCode m_charIterStatus;

	icu::BreakIterator* m_wordIter;
	UErrorCode m_wordIterStatus;

	icu::BreakIterator* m_lineIter;
	UErrorCode m_lineIterStatus;
};

#endif
