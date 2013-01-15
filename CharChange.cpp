// CharChange.cpp

#include "CharChange.h"
#include <algorithm>

CharChange::CharChange()
  : m_start( 0 )
  , m_end( 0 )
  , m_delta( 0 )
{
}

CharChange::CharChange( size_t start, size_t count, Type type )
{
  if ( type == kInsertion )
  {
    m_start = start;
    m_end   = start;
    m_delta = static_cast<int32_t>( count );
  }
  else if ( type == kDeletion )
  {
    m_start = start;
    m_end   = m_start + count;
    m_delta = -static_cast<int32_t>( count );
  }
  else
  {
    m_start = start;
    m_end   = m_start + count;
    m_delta = 0;
  }
}

void CharChange::Add( const CharChange& change )
{
  if ( m_start == m_end && m_delta == 0 )
  {
    m_start = change.start();
    m_end   = change.end();
    m_delta = change.delta();
  }
  else
  {
    size_t changeStart = change.start();
    size_t changeEnd   = change.end();

    if ( changeStart > m_start )
      changeStart -= std::min<int32_t>( changeStart - m_start, m_delta );

    if ( changeEnd > m_start )
      changeEnd -= std::min<int32_t>( changeEnd - m_start, m_delta );

    m_start = std::min( start, changeStart );
    m_end   = std::max( end, changeEnd );
    delta  += change.delta;
  }
}
