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

	CharChange Insert( size_t pos, ArrayRef<const wchar_t> text );
	CharChange Delete( size_t pos, size_t count );

	ArrayRef<const wchar_t> Read( size_t pos, size_t count ) const;

	void NewUndoGroup();

	bool CanUndo() const { return m_undoBuffer.CanUndo(); }
	bool CanRedo() const { return m_undoBuffer.CanRedo(); }

	UndoChange Undo();
	CharChange Redo();

private:
	CharBuffer m_charBuffer;
	LineBuffer m_lineBuffer;
	UndoBuffer m_undoBuffer;
};

#endif
