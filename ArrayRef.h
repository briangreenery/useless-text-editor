// ArrayRef.h

#ifndef ArrayRef_h
#define ArrayRef_h

#include <cassert>

// ArrayRef is a const reference to a section of an array.

template < class T >
struct ArrayRef
{
  ArrayRef()
    : start( 0 )
    , end( 0 )
  {}

  ArrayRef( const T* start, const T* end )
    : start( start )
    , end( end )
  {}

  ArrayRef( const ArrayRef& other )
    : start( other.start )
    , end( other.end )
  {}

  const T& operator[]( size_t i ) const
  {
    assert( i < Length() );
    return start[i];
  }

  size_t Length() const { return end - start; }
  bool IsEmpty() const  { return end == start; }

  const T* start;
  const T* end;
};

#endif
