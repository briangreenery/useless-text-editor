// GapArray.h

#ifndef GapArray_h
#define GapArray_h

#include "ArrayRef.h"

// GapArray is an array where nearby inserts and deletes are very fast (it's on wikipedia).

class GapArrayBase
{
public:
	GapArrayBase();
	~GapArrayBase();

	void InsertBytes( size_t pos, ArrayRef<const uint8_t> bytes );
	void EraseBytes ( size_t pos, size_t count );
	ArrayRef<const uint8_t> ReadBytes( size_t pos, size_t count ) const;

protected:
	size_t GapLength() const { return m_capacity - m_size; }

	void MoveGapTo( size_t pos ) const;
	void Resize( size_t newCapacity );

	uint8_t* m_buffer;
	size_t m_size;
	size_t m_capacity;
	mutable size_t m_gapPosition;
};

template < class T >
class GapArray : private GapArrayBase
{
public:
	void Insert( size_t pos, const T& element );
	void Insert( size_t pos, ArrayRef<const T> elements );
	void Erase ( size_t pos, size_t count );
	ArrayRef<const T> Read( size_t pos, size_t count ) const;

	const T& operator[]( size_t pos ) const;

	size_t Length() const;
};

template < class T >
void GapArray<T>::Insert( size_t pos, const T& element )
{
	Insert( pos, ArrayRef<const T>( &element, 1 ) );
}

template < class T >
void GapArray<T>::Insert( size_t pos, ArrayRef<const T> elements )
{
	ArrayRef<const uint8_t> bytes( reinterpret_cast<const uint8_t*>( elements.begin() ),
	                               reinterpret_cast<const uint8_t*>( elements.end() ) );

	InsertBytes( pos * sizeof( T ), bytes );
}

template < class T >
void GapArray<T>::Erase( size_t pos, size_t count )
{
	EraseBytes( pos * sizeof( T ), count * sizeof( T ) );
}

template < class T >
ArrayRef<const T> GapArray<T>::Read( size_t pos, size_t count ) const
{
	ArrayRef<const uint8_t> bytes = ReadBytes( pos * sizeof( T ), count * sizeof( T ) );

	ArrayRef<const T> elements( reinterpret_cast<const T*>( bytes.begin() ),
	                            reinterpret_cast<const T*>( bytes.end() ) );

	return elements;
}

template < class T >
const T& GapArray<T>::operator[]( size_t pos ) const
{
	assert( pos < Length() );

	size_t bytePos = pos * sizeof( T );
	
	if ( bytePos < m_gapPosition )
		bytePos += m_gapPosition;

	return *reinterpret_cast<const T*>( m_buffer + bytePos );
}

template < class T >
size_t GapArray<T>::Length() const
{
	return m_size / sizeof( T );
}

#endif
