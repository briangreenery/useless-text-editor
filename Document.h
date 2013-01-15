// Document.h

#ifndef Document_h
#define Document_h

#include "CharBuffer.h"
#include "LineBuffer.h"
#include "UndoBuffer.h"

// Document stores all non-display, non-style stuff.

class Document
{
public:
  Document();

  CharChange Insert( size_t pos, ArrayRef<wchar_t> text );
  CharChange Delete( size_t pos, size_t count );

  ArrayRef<wchar_t> Read( size_t pos, size_t count ) const;

  void NewUndoGroup();

  bool CanUndo() const;
  bool CanRedo() const;

  UndoChange Undo();
  CharChange Redo();

  const CharBuffer& text() const  { return m_text; }
  const LineBuffer& lines() const { return m_lines; }

private:
  CharBuffer m_text;
  LineBuffer m_lines;
  UndoBuffer m_undo;
};

#endif
