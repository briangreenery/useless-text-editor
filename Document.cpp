// Document.cpp

#include "Document.h"

Document::Document()
  : m_lineBuffer( m_charBuffer )
  , m_undoBuffer( m_charBuffer )
{
}

namespace
{

const wchar_t* FindLineBreak( const wchar_t* it, const wchar_t* end )
{
  for ( ; it != end; ++it )
    if ( *it == 0x0A || *it == 0x0D )
      return it;

  return end;
}

const wchar_t* SkipLineBreak( const wchar_t* lineBreak, const wchar_t* end )
{
  if ( lineBreak == end )
    return end;

  if ( *lineBreak != 0x0D )
    return lineBreak + 1;

  ++lineBreak;

  if ( lineBreak != end && *lineBreak == 0x0A )
    return lineBreak + 1;

  return lineBreak;
}

}

CharChange Document::Insert( size_t pos, ArrayRef<wchar_t> text )
{
  if ( text.IsEmpty() )
    return CharChange();

  // This normalizes the line endings to 0x0A while inserting

  size_t numInserted = 0;

  for ( const wchar_t* it = text.start; it != text.end; )
  {
    const wchar_t* lineBreak = FindLineBreak( it, text.end );

    if ( !TryInsertWithRollback( pos, numInserted, it, lineBreak ) )
      return CharChange();

    numInserted += lineBreak - it;

    if ( lineBreak != text.end )
    {
      if ( !TryInsertWithRollback( pos, numInserted, 0x0A ) )
        return CharChange();

      numInserted += 1;
    }

    it = SkipLineBreak( lineBreak, text.end );
  }

  return CharChange( pos, numInserted, CharChange::kInsertion );
}

CharChange Document::Delete( size_t pos, size_t count )
{
  if ( count == 0 )
    return CharChange();

  m_text.Delete( pos, count );
  return change;
}

ArrayRef<const wchar_t> Document::Read( size_t pos, size_t count ) const
{
  return m_charBuffer.Read( pos, count );
}

UndoChange Document::Undo()
{
  UndoChange change;
  return change;
}

CharChange Document::Redo()
{
  CharChange change;
  return change;
}
