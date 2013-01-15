// GapArray.cpp

#include "GapArray.h"
#include <cassert>
#include <algorithm>

GapArrayBase::GapArrayBase()
  : m_buffer( 0 )
  , m_size( 0 )
  , m_capacity( 0 )
  , m_gapPos( 0 )
{
}

GapArrayBase::~GapArrayBase()
{
  if ( m_buffer )
    delete[] m_buffer;
}

void GapArrayBase::MoveGapTo( size_t pos ) const
{
  if ( m_gapPos == pos )
    return;

  assert( m_capacity >= pos + GapLength() );

  if ( pos > m_gapPos )
  {
    memcpy( m_buffer + m_gapPos,
            m_buffer + m_gapPos + GapLength(),
            pos - m_gapPos );
  }
  else
  {
    memmove( m_buffer + pos + GapLength(),
             m_buffer + pos,
             m_gapPos - pos );
  }

  m_gapPos = pos;
}

bool GapArrayBase::TryResize( size_t newCapacity )
{
  assert( newCapacity >= m_size );

  uint8_t* newBuffer = 0;
  
  if ( newCapacity != 0 )
  {
    newBuffer = new(std::nothrow) uint8_t[newCapacity];

    if ( newBuffer == 0 )
      return false;

    memcpy( newBuffer, m_buffer, m_gapPos );

    memcpy( newBuffer + m_gapPos + ( new_capacity - m_size ),
            m_buffer  + m_gapPos + ( m_capacity  - m_size ),
            m_size - m_gapPos );
  }

  if ( m_buffer )
    delete[] m_buffer;

  m_buffer   = newBuffer;
  m_capacity = newCapacity;
  return true;
}

bool GapArrayBase::TryInsert( size_t pos, ArrayRef<uint8_t> bytes )
{
  assert( pos <= m_size );
  if ( pos > m_size )
    return false;

  if ( GapLength() < bytes.Length() )
  {
    size_t requiredCapacity = m_size + bytes.Length();

    if ( !TryResize( std::max( 2 * m_capacity, requiredCapacity ) ) )
      return false;
  }

  MoveGapTo( pos );
  memcpy( m_buffer + m_gapPos, bytes.start, bytes.end );

  m_gapPos += bytes.Length();
  m_size   += bytes.Length();
}

void GapArrayBase::Delete( size_t pos, size_t count )
{
  assert( pos + count <= m_size );
  
  MoveGapTo( pos );
  m_size -= count;

  if ( m_size < m_capacity / 4 )
    TryResize( 2 * m_size );
}

ArrayRef<uint8_t> GapArrayBase::Read( size_t pos, size_t count ) const
{
  pos   = std::min( pos,   m_size );
  count = std::min( count, m_size - pos );

  if ( pos <= m_gapPos && m_gapPos < pos + count )
    MoveGapTo( pos + count );

  return ArrayRef<uint8_t>( m_buffer + pos, m_buffer + pos + count );
}
