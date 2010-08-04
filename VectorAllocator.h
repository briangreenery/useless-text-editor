// VectorAllocator.h

#ifndef VectorAllocator_h
#define VectorAllocator_h

#include "SizedAutoArray.h"
#include "ArrayOf.h"
#include "Assert.h"
#include <algorithm>

template < class T >
class VectorAllocator
{
public:
	VectorAllocator()
		: m_freeStart( 0 )
	{}

	void Reset( size_t );
	SizedAutoArray<T> Finish();

	ArrayOf<T> Allocated() const;

	ArrayOf<T> Alloc (             size_t );
	ArrayOf<T> Shrink( ArrayOf<T>, size_t );
	ArrayOf<T> Grow  ( ArrayOf<T>, size_t );

	void PushBack( T );

	void DiscardFrom( size_t );
	void DiscardFrom( T*     );
	void DiscardAll();

	size_t MakeRelative( T* p ) const          { return p - m_buffer.begin(); }
	T*     MakeAbsolute( size_t offset ) const { return m_buffer.begin() + offset; }

private:
	SizedAutoArray<T> m_buffer;
	T* m_freeStart;
};

template < class T >
void VectorAllocator<T>::Reset( size_t initialSize )
{
	if ( m_buffer.size() < initialSize )
		m_buffer = CreateArray<T>( initialSize );

	m_freeStart = m_buffer.begin();
}

template < class T >
SizedAutoArray<T> VectorAllocator<T>::Finish()
{
	if ( m_freeStart == m_buffer.begin() )
		return SizedAutoArray<T>();

	SizedAutoArray<T> result = CreateArray<T>( m_freeStart - m_buffer.begin() );
	std::copy( m_buffer.begin(), m_freeStart, result.begin() );
	m_freeStart = m_buffer.begin();

	return result;
}

template < class T >
ArrayOf<T> VectorAllocator<T>::Allocated() const
{
	return ArrayOf<T>( m_buffer.begin(), m_freeStart );
}

template < class T >
ArrayOf<T> VectorAllocator<T>::Alloc( size_t requestSize )
{
	if ( m_freeStart + requestSize <= m_buffer.end() )
	{
		T* start = m_freeStart;
		return ArrayOf<T>( start, m_freeStart += requestSize );
	}

	size_t freeStart = m_freeStart - m_buffer.begin();
	size_t newSize   = m_buffer.size() + (std::max)( m_buffer.size(), requestSize );

	SizedAutoArray<T> newBuffer = CreateArray<T>( newSize );
	std::copy( m_buffer.begin(), m_freeStart, newBuffer.begin() );

	m_buffer    = newBuffer;
	m_freeStart = m_buffer.begin() + freeStart;

	T* start = m_freeStart;
	return ArrayOf<T>( start, m_freeStart += requestSize );
}

template < class T >
ArrayOf<T> VectorAllocator<T>::Shrink( ArrayOf<T> a, size_t size )
{
	Assert( m_buffer.begin() <= a.begin() && a.end() <= m_buffer.end() );
	Assert( m_freeStart == a.end() );
	Assert( size <= a.size() );

	m_freeStart = a.begin() + size;
	return ArrayOf<T>( a.begin(), m_freeStart );
}

template < class T >
ArrayOf<T> VectorAllocator<T>::Grow( ArrayOf<T> a, size_t size )
{
	Assert( m_buffer.begin() <= a.begin() && a.end() <= m_buffer.end() );
	Assert( m_freeStart == a.end() );

	m_freeStart = a.begin();
	return Alloc( a.size() + size );
}

template < class T >
void VectorAllocator<T>::PushBack( T t )
{
	Alloc( 1 )[0] = t;
}

template < class T> 
void VectorAllocator<T>::DiscardFrom( size_t point )
{
	Assert( m_buffer.begin() + point <= m_freeStart );
	m_freeStart = m_buffer.begin() + point;
}

template < class T >
void VectorAllocator<T>::DiscardFrom( T* point )
{
	Assert( point <= m_freeStart );
	m_freeStart = point;
}

template < class T >
void VectorAllocator<T>::DiscardAll()
{
	m_freeStart = m_buffer.begin();
}

#endif
