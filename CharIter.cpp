// CharIter.cpp

#include "CharIter.h"
#include <algorithm>

CharIter::CharIter( const GapArray<wchar_t>& buffer )
	: CharacterIterator( buffer.Length(), 0, buffer.Length(), 0 )
	, m_buffer( buffer )
{
}

UClassID CharIter::getDynamicClassID() const
{
	static char classid = 0;
	return &classid;
}

UBool CharIter::operator==( const icu::ForwardCharacterIterator& other ) const
{
	const CharIter* iter = dynamic_cast<const CharIter*>( &other );
	return iter && pos == iter->pos && &m_buffer == &iter->m_buffer;
}

int32_t CharIter::hashCode() const
{
	return 0;
}

icu::CharacterIterator* CharIter::clone() const
{
	CharIter* cloned = new CharIter( m_buffer );
	cloned->pos = pos;
	return cloned;
}

UChar CharIter::first()
{
	return CharIter::setIndex( 0 );
}

UChar32 CharIter::first32()
{
	return CharIter::setIndex32( 0 );
}

UChar CharIter::last()
{
	pos = textLength;

	if ( pos == 0 )
		return DONE;

	return m_buffer[--pos];
}

UChar32 CharIter::last32()
{
	pos = textLength;

	if ( pos == 0 )
		return DONE;
	
	UChar32 result;
	U16_PREV( m_buffer, 0, pos, result );
	return result;
}

UChar CharIter::setIndex( int32_t position )
{
	pos = position;
	return CharIter::current();
}

UChar32 CharIter::setIndex32( int32_t position )
{
	pos = position;
	return CharIter::current32();
}

UChar CharIter::current() const
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	return m_buffer[pos];
}

UChar32 CharIter::current32() const
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	UChar32 result;
	U16_GET( m_buffer, 0, pos, textLength, result );
	return result;
}

UChar CharIter::next()
{
	if ( pos < -1 || pos >= textLength - 1 )
		return DONE;

	return m_buffer[++pos];
}

UChar32 CharIter::next32()
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	U16_FWD_1( m_buffer, pos, textLength );
	return CharIter::current32();
}

UChar CharIter::nextPostInc()
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	return m_buffer[pos++];
}

UChar32 CharIter::next32PostInc()
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	UChar32 result;
	U16_NEXT( m_buffer, pos, textLength, result );
	return result;
}

UBool CharIter::hasNext()
{
	return 0 <= pos && pos < textLength;
}

UChar CharIter::previous()
{
	if ( pos <= 0 || pos > textLength )
		return DONE;

	return m_buffer[--pos];
}

UChar32 CharIter::previous32()
{
	if ( pos <= 0 || pos > textLength )
		return DONE;

	UChar32 result;
	U16_PREV( m_buffer, 0, pos, result );
	return result;
}

UBool CharIter::hasPrevious()
{
	return 0 < pos && pos <= textLength;
}

int32_t CharIter::move( int32_t delta, EOrigin origin )
{
	switch ( origin )
	{
	case kStart: pos = 0;          break;
	case kEnd:   pos = textLength; break;
	}

	pos += delta;
	return pos;
}

int32_t CharIter::move32( int32_t delta, EOrigin origin )
{
	switch ( origin )
	{
	case kStart: pos = 0;          break;
	case kEnd:   pos = textLength; break;
	}

	if ( delta > 0 )
	{
		if ( pos >= 0 )
		{
			U16_FWD_N( m_buffer, pos, textLength, delta );
		}
	}
	else
	{
		if ( pos <= textLength )
		{
			delta = -delta;
			U16_BACK_N( m_buffer, 0, pos, delta );
		}
	}

	return pos;
}

void CharIter::getText( UnicodeString& )
{
	assert( false );
}
