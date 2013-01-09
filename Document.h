// Document.h

#ifndef Document_h
#define Document_h

#include "CharBuffer.h"
#include "LineBuffer.h"
#include "UndoBuffer.h"
#include "CharChange.h"

class Document
{
public:
	CharChange Insert( size_t pos, ArrayRef<const wchar_t> text );
	CharChange Delete( size_t pos, size_t count );

	void NewUndoGroup( CharRange selection );

	bool CanUndo() const { return m_undoBuffer.CanUndo(); }
	bool CanRedo() const { return m_undoBuffer.CanRedo(); }

	UndoChange Undo();
	CharChange Redo();

private:
	CharBuffer m_charBuffer;
	LineBuffer m_lineBuffer;
	UndoBuffer m_undoBufer;
};

#endif
