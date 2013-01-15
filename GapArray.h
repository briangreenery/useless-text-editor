// GapArray.h

#ifndef GapArray_h
#define GapArray_h

#include "ArrayRef.h"

// GapArray is an array where clustered inserts and deletes are very fast.
// It's on wikipedia.

class GapArrayBase
{
 public:
  GapArrayBase();
  ~GapArrayBase();

  bool TryInsert( size_t pos, ArrayRef<uint8_t> );
  void Delete( size_t pos, size_t count );
  ArrayRef<uint8_t> Read( size_t pos, size_t count ) const;

  const uint8_t* buffer() const { return m_buffer; }
  size_t size() const           { return m_size; }
  size_t capacity() const       { return m_capacity; }
  size_t gapPos() const         { return m_gapPos; }

 private:
  size_t GapLength() const { return m_capacity - m_size; }
  void MoveGapTo( size_t pos ) const;

  bool TryResize( size_t newCapacity );

  uint8_t* m_buffer;
  size_t m_size;
  size_t m_capacity;
  mutable size_t m_gapPos;
};

template < class T >
class GapArray
{
public:
  bool TryInsert( size_t pos, ArrayRef<T> elements );
  void Delete( size_t pos, size_t count );
  ArrayRef<T> Read( size_t pos, size_t count ) const;

  const T& operator[]( size_t pos ) const;

  size_t Length() const;

private:
  GapArrayBase m_bytes;
};

template < class T >
bool GapArray<T>::TryInsert( size_t pos, ArrayRef<T> elements )
{
  ArrayRef<uint8_t> bytes( reinterpret_cast<const uint8_t*>( elements.start ),
                           reinterpret_cast<const uint8_t*>( elements.end ) );

  return m_bytes.TryInsert( pos * sizeof( T ), bytes );
}

template < class T >
void GapArray<T>::Delete( size_t pos, size_t count )
{
  m_bytes.Delete( pos * sizeof( T ), count * sizeof( T ) );
}

template < class T >
ArrayRef<T> GapArray<T>::Read( size_t pos, size_t count ) const
{
  ArrayRef<uint8_t> bytes = m_bytes.Read( pos * sizeof( T ),
                                          count * sizeof( T ) );

  return ArrayRef<T>( reinterpret_cast<const T*>( bytes.start ),
                      reinterpret_cast<const T*>( bytes.end ) );
}

template < class T >
const T& GapArray<T>::operator[]( size_t pos ) const
{
  assert( pos < Length() );

  size_t bytePos = pos * sizeof( T );
  
  if ( bytePos < m_bytes.gapPos() )
    bytePos += m_bytes.gapPos();

  return *reinterpret_cast<T*>( m_bytes.buffer() + bytePos );
}

template < class T >
size_t GapArray<T>::Length() const
{
  return m_bytes.size() / sizeof( T );
}

#endif
