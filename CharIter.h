// CharIter.h

#ifndef CharIter_h
#define CharIter_h

#include "GapArray.h"
#include <unicode/chariter.h>

// CharIter is used by ICU to read parts of the text.
// ICU is used to find character, word, and soft-linebreak boundaries.

class CharIter : public icu::CharacterIterator
{
public:
	CharIter( const GapArray<wchar_t>& );

	virtual UChar   first();
	virtual UChar32 first32();

	virtual UChar   last();
	virtual UChar32 last32();

	virtual UChar   setIndex  ( int32_t position );
	virtual UChar32 setIndex32( int32_t position );

	virtual UChar   current() const;
	virtual UChar32 current32() const;

	virtual UChar   next();
	virtual UChar32 next32();
	virtual UChar   nextPostInc();
	virtual UChar32 next32PostInc();
	virtual UBool   hasNext();

	virtual UChar   previous();
	virtual UChar32 previous32();
	virtual UBool   hasPrevious();

	virtual int32_t move  ( int32_t delta, EOrigin origin );
	virtual int32_t move32( int32_t delta, EOrigin origin );

	virtual void getText( UnicodeString& );

	virtual UClassID getDynamicClassID() const;
	virtual UBool operator==( const icu::ForwardCharacterIterator& ) const;
	virtual int32_t hashCode() const;
	virtual CharacterIterator* clone() const;

private:
	const GapArray<wchar_t>& m_buffer;
};

#endif
