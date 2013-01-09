// UndoBuffer.h

#ifndef UndoBuffer_h
#define UndoBuffer_h

#include "CharChange.h"
#include "CharRange.h"
#include <vector>

class CharBuffer;

class UndoChange
{
public:
	CharChange change;
	CharRange  selection;
};

class UndoAction
{
public:
	enum Type { insertion, deletion };

	UndoAction( Type type, size_t pos, size_t count, size_t savedTextPos );

	Type   type;
	size_t pos;
	size_t count;
	size_t savedTextPos;
};

class UndoGroup
{
public:
	UndoGroup( CharRange selection );

	void RecordInsertion( CharBuffer&, size_t pos, size_t length, size_t savedTextPos );
	void RecordDeletion ( CharBuffer&, size_t pos, size_t length, size_t savedTextPos );

	UndoChange Undo( CharBuffer&, UTF16Ref savedText ) const;
	CharChange Redo( CharBuffer&, UTF16Ref savedText ) const;

private:
	std::vector<UndoAction> m_actions;
	CharRange m_selection;
};

class UndoBuffer
{
public:
	UndoBuffer( CharBuffer& );

	void AddAction( UndoAction );

	bool CanUndo() const;
	bool CanRedo() const;

	UndoChange Undo( CharBuffer& );
	CharChange Redo( CharBuffer& );

	void SetSelection( CharRange );
	void EndGroup();

private:
	CharBuffer& m_charBuffer;

	bool m_endCurrentGroup;

	CharRange m_beforeSelection;

	size_t m_index;
	std::vector<UndoGroup> m_groups;
	std::vector<wchar_t> m_savedText;
};

#endif
