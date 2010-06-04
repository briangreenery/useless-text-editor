// ArrayOf.h

#ifndef ArrayOf_h
#define ArrayOf_h

#include <cstddef>

template < class T >
class ArrayOf
{
public:
	ArrayOf()
		: m_begin( 0 )
		, m_end( 0 )
	{
	}

	ArrayOf( T* begin, T* end )
		: m_begin( begin )
		, m_end( end )
	{
	}

	ArrayOf( T* begin, size_t size )
		: m_begin( begin )
		, m_end( begin + size )
	{
	}

	ArrayOf( const ArrayOf& other )
		: m_begin( other.m_begin )
		, m_end( other.m_end )
	{
	}

	ArrayOf& operator=( const ArrayOf& other )
	{
		m_begin = other.m_begin;
		m_end   = other.m_end;
		return *this;
	}

	T* begin() const                { return m_begin; }
	T* end() const                  { return m_end; }

	T* rbegin() const               { return m_end - 1; }
	T* rend() const                 { return m_begin - 1; }

	size_t size() const             { return m_end - m_begin; }
	bool empty() const              { return m_begin == m_end; }

	T& operator[]( size_t i ) const { return m_begin[i]; }

private:
	T* m_begin;
	T* m_end;
};

#endif
