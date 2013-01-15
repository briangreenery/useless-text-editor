// CharChange.h

#ifndef CharChange_h
#define CharChange_h

#include <stdint.h>

// CharChange collects a group of changes and calculates the start, end, and
// delta of those changes.

class CharChange
{
 public:
  enum Type { kInsertion, kDeletion, kModification };

  CharChange();
  CharChange( size_t start, size_t count, Type type );

  void Add( const CharChange& );

  size_t  start() const { return m_start; }
  size_t  end() const   { return m_end; }
  int32_t delta() const { return m_delta; }

 private:
  size_t  m_start;
  size_t  m_end;
  int32_t m_delta;
};

#endif
