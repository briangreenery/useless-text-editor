// Error.h

#ifndef Error_h
#define Error_h

#include "Assert.h"

namespace Windows
{

class BadHRESULT {};

inline void ThrowHRESULT( HRESULT hResult )
{
	if ( hResult != S_OK )
	{
		Assert( false );
		throw BadHRESULT();
	}
}

}

#endif
