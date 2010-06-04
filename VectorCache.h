// VectorCache.h

#ifndef VectorCache_h
#define VectorCache_h

#include "SizedAutoArray.h"

template < class T >
class VectorCache
{
public:
	ArrayOf<T> TempArray( size_t );

private:
	SizedAutoArray<T> m_cache;
};

template < class T> 
ArrayOf<T> VectorCache<T>::TempArray( size_t requestSize )
{
	if ( m_cache.size() < requestSize )
		m_cache = CreateArray<T>( requestSize );

	return ArrayOf<T>( m_cache.begin(), requestSize );
}

#endif
