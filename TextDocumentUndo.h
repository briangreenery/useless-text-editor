// TextDocumentUndo.h

#ifndef TextDocumentUndo_h
#define TextDocumentUndo_h

#include "UString.h"
#include "TextChange.h"
#include <vector>

class TextDocument;

class TextDocumentUndo
{
public:
	TextDocumentUndo();

	void RecordInsertion( TextDocument&, size_t pos, size_t length );
	void RecordDeletion ( TextDocument&, size_t pos, size_t length );

	bool CanUndo() const;
	bool CanRedo() const;

	TextChange Undo( TextDocument& );
	TextChange Redo( TextDocument& );

	void StopGrouping();

private:
	size_t SaveText( TextDocument&, size_t pos, size_t length );
	UTF16Ref SavedText( size_t pos, size_t length ) const;

	void RemoveUnreachableActions();

	struct TextUndoAction
	{
		enum Type { insertion, deletion } type;
		size_t pos;
		size_t length;
		size_t savedTextPos;
	};

	bool m_stopGrouping;

	size_t m_index;
	std::vector<TextUndoAction> m_actions;

	std::vector<UTF16::Unit> m_savedText;

	bool m_undoingOrRedoing;
};

#endif
