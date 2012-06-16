// Error.h

#ifndef Error_h
#define Error_h

#include <cassert>

namespace Windows
{

class BadHRESULT {};

inline void ThrowHRESULT( HRESULT hResult )
{
	if ( hResult != S_OK )
	{
		assert( false );
		throw BadHRESULT();
	}
}

}

#endif
