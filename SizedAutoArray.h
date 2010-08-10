// SizedAutoArray.h

#ifndef SizedAutoArray_h
#define SizedAutoArray_h

#include "ArrayOf.h"

template < class T >
class SizedAutoArray
{
public:
	SizedAutoArray()
		: m_begin( 0 )
		, m_end( 0 )
	{
	}

	SizedAutoArray( T* begin, T* end )
		: m_begin( begin )
		, m_end( end )
	{
	}

	SizedAutoArray( T* begin, size_t size )
		: m_begin( begin )
		, m_end( begin + size )
	{
	}

	SizedAutoArray( SizedAutoArray& other )
		: m_begin( other.m_begin )
		, m_end( other.m_end )
	{
		other.m_begin = 0;
		other.m_end   = 0;
	}

	SizedAutoArray& operator=( SizedAutoArray& other )
	{
		if ( this != &other )
		{
			if ( m_begin )
				delete[] m_begin;

			m_begin = other.m_begin;
			m_end   = other.m_end;
			other.m_begin = 0;
			other.m_end   = 0;
		}

		return *this;
	}

	~SizedAutoArray()
	{
		if ( m_begin )
			delete[] m_begin;
	}

	T* begin() const                 { return m_begin; }
	T* end() const                   { return m_end; }
	size_t size() const              { return m_end - m_begin; }
	bool empty() const               { return m_begin == m_end; }

	T* rbegin() const                { return m_end - 1; }
	T* rend() const                  { return m_begin - 1; }

	T& operator[] ( size_t i ) const { return m_begin[i]; }

	void Reset()                     { if ( m_begin ) delete[] m_begin; m_begin = m_end = 0; }
	T* Release();
	operator ArrayOf<T>() const      { return ArrayOf<T>( m_begin, m_end ); }

private:
	T* m_begin;
	T* m_end;
};

template < class T >
T* SizedAutoArray<T>::Release()
{
	T* result = m_begin;
	m_begin = m_end = 0;
	return result;
}

template < class T >
SizedAutoArray<T> CreateArray( size_t size )
{
	T* begin = new T[size];
	return SizedAutoArray<T>( begin, begin + size );
}

#endif
