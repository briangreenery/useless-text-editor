// CharRange.h

#ifndef CharRange_h
#define CharRange_h

#include <vector>

// CharRange is a range of text.

struct CharRange
{
  CharRange()
    , start( 0 )
    , count( 0 )
  {}

  CharRange( size_t start, size_t count )
    : start( start )
    , count( count )
  {}

  size_t start;
  size_t count;
};

typedef std::vector<CharRange> CharRanges;

#endif
