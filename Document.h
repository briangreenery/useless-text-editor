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
	Document();
	~Document();

	CharChange Insert( size_t pos, const wchar_t* text, size_t count );
	CharChange Delete( size_t pos, size_t count );

	void SetUndoSelection( CharRange selection ) { m_undoBuffer.SetSelection( selection ); }
	void EndUndoGroup()                          { m_undoBuffer.EndGroup(); }

	bool CanUndo() const                         { return m_undoBuffer.CanUndo(); }
	bool CanRedo() const                         { return m_undoBuffer.CanRedo(); }

	UndoChange Undo();
	CharChange Redo();

private:
	CharBuffer m_charBuffer;
	LineBuffer m_lineBuffer;
	UndoBuffer m_undoBufer;
};

#endif
