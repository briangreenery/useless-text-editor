// GapArray.h

#ifndef GapArray_h
#define GapArray_h

#include <stdint.h>

class GapArrayBase
{
public:
	GapArrayBase();
	~GapArrayBase();

	void InsertBytes( size_t pos, const uint8_t* buffer, size_t count );
	void EraseBytes( size_t pos, size_t count );

	size_t ReadBytes( size_t pos, uint8_t* buffer, size_t count ) const;

protected:
	size_t GapLength() const { return m_capacity - m_size; }

	void MoveGapTo( size_t pos );
	void Resize( size_t newCapacity );

	uint8_t* m_buffer;
	size_t m_size;
	size_t m_capacity;
	size_t m_gapPosition;
};

template < class T >
class GapArray : private GapArrayBase
{
public:
	void Insert( size_t pos, const T& element );
	void Insert( size_t pos, const T* elements, size_t count );
	void Erase( size_t pos, size_t count );

	size_t Read( size_t pos, T* elements, size_t count ) const;

	const T& operator[]( size_t pos ) const;

	bool   IsEmpty() const { return m_size == 0; }
	size_t Size() const    { return m_size / sizeof( T ); }
};

template < class T >
void GapArray<T>::Insert( size_t pos, const T& element )
{
	Insert( pos, &element, 1 );
}

template < class T >
void GapArray<T>::Insert( size_t pos, const T* elements, size_t count )
{
	InsertBytes( pos * sizeof( T ), elements, count * sizeof( T ) );
}

template < class T >
void GapArray<T>::Erase( size_t pos, size_t count )
{
	EraseBytes( pos * sizeof( T ), count * sizeof( T ) );
}

template < class T >
size_t GapArray<T>::Read( size_t pos, T* elements, size_t count ) const
{
	return ReadBytes( pos * sizeof( T ), elements, count * sizeof( T ) ) / sizeof( T );
}

template < class T >
const T& GapArray<T>::operator[]( size_t pos ) const
{
	assert( pos < Size() );

	size_t bytePos = pos * sizeof( T );
	
	if ( bytePos < m_gapPosition )
		bytePos += m_gapPosition;

	return *reinterpret_cast<const T*>( m_buffer + bytePos );
}

#endif
