// ThrowHRESULT.h

#ifndef ThrowHRESULT_h
#define ThrowHRESULT_h

#include <cassert>

class BadHRESULT {};

inline void ThrowHRESULT( HRESULT hResult )
{
	if ( hResult != S_OK )
	{
		assert( false );
		throw BadHRESULT();
	}
}

#endif
