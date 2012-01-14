// TextDocumentUndo.h

#ifndef TextDocumentUndo_h
#define TextDocumentUndo_h

#include "UString.h"
#include "TextChange.h"
#include "TextSelection.h"
#include <vector>

class TextDocument;

struct TextUndoAction
{
	enum Type { insertion, deletion };

	TextUndoAction( Type type, size_t pos, size_t length, size_t savedTextPos );

	Type type;
	size_t pos;
	size_t length;
	size_t savedTextPos;
};

class TextUndoGroup
{
public:
	TextUndoGroup( const TextSelection& );

	void RecordInsertion( TextDocument&, size_t pos, size_t length, size_t savedTextPos );
	void RecordDeletion ( TextDocument&, size_t pos, size_t length, size_t savedTextPos );

	std::pair<TextChange,TextSelection> Undo( TextDocument&, UTF16Ref savedText ) const;
	TextChange Redo( TextDocument&, UTF16Ref savedText ) const;

	std::vector<TextUndoAction> m_actions;
	TextSelection m_beforeSelection;
};

class TextDocumentUndo
{
public:
	TextDocumentUndo();

	void RecordInsertion( TextDocument&, size_t pos, size_t length );
	void RecordDeletion ( TextDocument&, size_t pos, size_t length );

	bool CanUndo() const;
	bool CanRedo() const;

	std::pair<TextChange,TextSelection> Undo( TextDocument& );
	TextChange                          Redo( TextDocument& );

	void SetBeforeSelection( const TextSelection& );
	void StopGrouping();

private:
	size_t SaveText( TextDocument&, size_t pos, size_t length );

	void RemoveUnreachableGroups();

	bool m_stopGrouping;
	bool m_undoingOrRedoing;

	TextSelection m_beforeSelection;

	size_t m_index;
	std::vector<TextUndoGroup> m_groups;

	std::vector<UTF16::Unit> m_savedText;
};

#endif
