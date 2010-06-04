// TextDocument.h

#ifndef TextDocument_h
#define TextDocument_h

#include "TextChange.h"
#include "GapBuffer.h"
#include "UString.h"

class TextDocument
{
public:
	size_t Length() const;

	UTF16::Unit CharAt( size_t pos ) const;
	size_t Read( size_t start, size_t count, UTF16::Unit* dest ) const;

	TextChange Insert( size_t pos, UTF16Ref );
	TextChange Delete( size_t pos, size_t count );

	void CopyToClipboard( size_t start, size_t count ) const;

	//TextChange Undo();
	//TextChange Redo();

	//bool CanUndo() const;
	//bool CanRedo() const;

	//void BeginUndoGroup();
	//void EndUndoGroup();

private:
	size_t ReadWithCRLFSize( size_t start, size_t count ) const;
	void   ReadWithCRLF    ( size_t start, size_t count, ArrayOf<UTF16::Unit> out ) const;

	GapBuffer<UTF16::Unit> m_buffer;
};

#endif
