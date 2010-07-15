// DocumentCharIter.h

#ifndef DocumentCharIter_h
#define DocumentCharIter_h

#include <unicode/chariter.h>

class TextDocument;

class DocumentCharIter : public icu::CharacterIterator
{
public:
	DocumentCharIter( const TextDocument& );

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
	bool PositionInBounds() const;

	const TextDocument& m_doc;
	static char m_classid;
};

#endif
