// CharSelection.h

#ifndef CharSelection_h
#define CharSelection_h

// CharSelection is a range of text where the end can be less than the start.

struct CharSelection
{
  CharSelection()
    : start( 0 )
    , end( 0 )
  {}

  CharSelection( size_t start, size_t end )
    : start( start )
    , end( end )
  {}

  size_t Min() const;
  size_t Max() const;
  size_t Length() const;
  bool IsEmpty() const;
  bool Intersects( size_t start, size_t end ) const;

  size_t start;
  size_t end;
};

#endif
