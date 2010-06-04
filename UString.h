// UString.h

#ifndef UString_h
#define UString_h

#include <cstddef>

template < class Encoding >
class UStringRef
{
public:
	typedef typename Encoding::Unit Unit;

	UStringRef()
		: b( 0 )
		, e( 0 )
	{}

	UStringRef( const Unit* theBegin, const Unit* theEnd )
		: b( theBegin )
		, e( theEnd )
	{}

	UStringRef( const Unit* theBegin, size_t size )
		: b( theBegin )
		, e( theBegin + size )
	{}

	const Unit* begin() const { return b; }
	const Unit* end() const   { return e; }

	bool empty() const        { return b == e; }
	size_t size() const       { return e - b; }

private:
	const Unit* b;
	const Unit* e;
};

struct UTF16
{
	typedef unsigned short Unit;
};

typedef UStringRef<UTF16> UTF16Ref;

#endif
