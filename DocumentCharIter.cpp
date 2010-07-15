// DocumentCharIter.cpp

#include "DocumentCharIter.h"
#include "TextDocument.h"
#include <algorithm>

DocumentCharIter::DocumentCharIter( const TextDocument& doc )
	: CharacterIterator( doc.Length(), 0, doc.Length(), 0 )
	, m_doc( doc )
{
}

UClassID DocumentCharIter::getDynamicClassID() const
{
	return &m_classid;
}

UBool DocumentCharIter::operator==( const icu::ForwardCharacterIterator& other ) const
{
	const DocumentCharIter* iter = dynamic_cast<const DocumentCharIter*>( &other );
	return iter && pos == iter->pos && &m_doc == &iter->m_doc;
}

int32_t DocumentCharIter::hashCode() const
{
	return 0;
}

icu::CharacterIterator* DocumentCharIter::clone() const
{
	DocumentCharIter* cloned = new DocumentCharIter( m_doc );
	cloned->pos = pos;
	return cloned;
}

UChar DocumentCharIter::first()
{
	return DocumentCharIter::setIndex( 0 );
}

UChar32 DocumentCharIter::first32()
{
	return DocumentCharIter::setIndex32( 0 );
}

UChar DocumentCharIter::last()
{
	pos = textLength;

	if ( pos == 0 )
		return DONE;

	return m_doc[--pos];
}

UChar32 DocumentCharIter::last32()
{
	pos = textLength;

	if ( pos == 0 )
		return DONE;
	
	UChar32 result;
	U16_PREV( m_doc, 0, pos, result );
	return result;
}

UChar DocumentCharIter::setIndex( int32_t position )
{
	pos = position;
	return DocumentCharIter::current();
}

UChar32 DocumentCharIter::setIndex32( int32_t position )
{
	pos = position;
	return DocumentCharIter::current32();
}

UChar DocumentCharIter::current() const
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	return m_doc[pos];
}

UChar32 DocumentCharIter::current32() const
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	UChar32 result;
	U16_GET( m_doc, 0, pos, textLength, result );
	return result;
}

UChar DocumentCharIter::next()
{
	if ( pos < -1 || pos >= textLength - 1 )
		return DONE;

	return m_doc[++pos];
}

UChar32 DocumentCharIter::next32()
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	U16_FWD_1( m_doc, pos, textLength );
	return DocumentCharIter::current32();
}

UChar DocumentCharIter::nextPostInc()
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	return m_doc[pos++];
}

UChar32 DocumentCharIter::next32PostInc()
{
	if ( pos < 0 || pos >= textLength )
		return DONE;

	UChar32 result;
	U16_NEXT( m_doc, pos, textLength, result );
	return result;
}

UBool DocumentCharIter::hasNext()
{
	return 0 <= pos && pos < textLength;
}

UChar DocumentCharIter::previous()
{
	if ( pos <= 0 || pos > textLength )
		return DONE;

	return m_doc[--pos];
}

UChar32 DocumentCharIter::previous32()
{
	if ( pos <= 0 || pos > textLength )
		return DONE;

	UChar32 result;
	U16_PREV( m_doc, 0, pos, result );
	return result;
}

UBool DocumentCharIter::hasPrevious()
{
	return 0 < pos && pos <= textLength;
}

int32_t DocumentCharIter::move( int32_t delta, EOrigin origin )
{
	switch ( origin )
	{
	case kStart: pos = 0;          break;
	case kEnd:   pos = textLength; break;
	}

	pos += delta;
	return pos;
}

int32_t DocumentCharIter::move32( int32_t delta, EOrigin origin )
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
			U16_FWD_N( m_doc, pos, textLength, delta );
		}
	}
	else
	{
		if ( pos <= textLength )
		{
			delta = -delta;
			U16_BACK_N( m_doc, 0, pos, delta );
		}
	}

	return pos;
}

#include <Windows.h>

void DocumentCharIter::getText( UnicodeString& string )
{
	DebugBreak();
}

char DocumentCharIter::m_classid;
