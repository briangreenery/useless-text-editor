// CharBuffer.cpp

#include "CharBuffer.h"
#include "CharIter.h"
#include "CharRange.h"
#include "CharChange.h"

CharBuffer::CharBuffer()
	: m_charIterStatus( U_ZERO_ERROR )
	, m_wordIterStatus( U_ZERO_ERROR )
	, m_lineIterStatus( U_ZERO_ERROR )
	, m_charIter( icu::BreakIterator::createCharacterInstance( icu::Locale::getUS(), m_charIterStatus ) )
	, m_wordIter( icu::BreakIterator::createWordInstance     ( icu::Locale::getUS(), m_wordIterStatus ) )
	, m_lineIter( icu::BreakIterator::createLineInstance( icu::Locale::getDefault(), m_lineIterStatus ) )
	, m_needIterReset( true )
{
}

CharBuffer::~CharBuffer()
{
	// These use ICU's allocator
	delete m_charIter;
	delete m_wordIter;
	delete m_lineIter;
}

CharChange CharBuffer::Insert( size_t pos, wchar_t character )
{
	m_buffer.Insert( pos, character );
	return CharChange( pos, 1, CharChange::insertion );
}

CharChange CharBuffer::Insert( size_t pos, ArrayRef<const wchar_t> text )
{
	m_buffer.Insert( pos, text );
	return CharChange( pos, text.size(), CharChange::insertion );
}

CharChange CharBuffer::Delete( size_t pos, size_t count )
{
	m_buffer.Erase( pos, count );
	return CharChange( pos, count, CharChange::deletion );
}

ArrayRef<const wchar_t> CharBuffer::Read( size_t start, size_t count ) const
{
	return m_buffer.Read( start, count );
}

void CharBuffer::ResetIterators() const
{
	if ( m_needIterReset )
	{
		m_needIterReset = false;

		// The 'adoptText' function takes ownership of the new iterator
		m_charIter->adoptText( new CharIter( m_buffer ) );
		m_wordIter->adoptText( new CharIter( m_buffer ) );
		m_lineIter->adoptText( new CharIter( m_buffer ) );
	}
}

size_t CharBuffer::NextBreak( icu::BreakIterator* iter, size_t pos ) const
{
	ResetIterators();
	int32_t result = iter->following( pos );
	return ( result == icu::BreakIterator::DONE ) ? pos : result;
}

size_t CharBuffer::PrevBreak( icu::BreakIterator* iter, size_t pos ) const
{
	ResetIterators();
	int32_t result = iter->preceding( pos );
	return ( result == icu::BreakIterator::DONE ) ? pos : result;
}

static bool IsWhitespace( wchar_t unit )
{
	return unit == ' ' || unit == '\t' || unit == '\n' || unit == '\r';
}

size_t CharBuffer::NextLineBreak( size_t pos ) const
{
	assert( pos < Length() );

	for ( size_t length = Length(); pos < length; ++pos )
		if ( m_buffer[pos] == 0x0A )
			break;

	return pos;
}

size_t CharBuffer::NextNonWhitespace( size_t pos ) const
{
	assert( pos < Length() );

	for ( size_t length = Length(); pos < length; ++pos )
		if ( !IsWhitespace( m_buffer[pos] ) )
			break;

	return pos;
}

size_t CharBuffer::PrevNonWhitespace( size_t pos ) const
{
	assert( pos <= Length() );

	for ( ; pos != 0; --pos )
		if ( !IsWhitespace( m_buffer[pos - 1] ) )
			break;

	return pos;
}

CharRange CharBuffer::WordAt( size_t pos ) const
{
	size_t start = pos;
	size_t end   = pos;

	bool leftSpace  = ( pos > 0 )        && IsWhitespace( m_buffer[pos - 1] );
	bool rightSpace = ( pos < Length() ) && IsWhitespace( m_buffer[pos] );

	if ( leftSpace && rightSpace )
	{
		start = PrevNonWhitespace( pos );
		end   = NextNonWhitespace( pos );
	}
	else if ( leftSpace && !rightSpace )
	{
		end = NextWordStop( pos );
	}
	else if ( !leftSpace && rightSpace )
	{
		end = PrevWordStop( pos );
	}
	else
	{
		ResetIterators();

		if ( pos == Length() || !m_wordIter->isBoundary( pos ) )
			start = PrevWordStop( pos );

		end = NextWordStop( pos );
	}

	return CharRange( start, end - start );
}
