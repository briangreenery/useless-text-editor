// TextDocument.h

#ifndef TextDocument_h
#define TextDocument_h

#include "TextChange.h"
#include "GapBuffer.h"
#include "UString.h"
#include <unicode/brkiter.h>

class TextDocument
{
public:
	TextDocument();
	~TextDocument();

	size_t Length() const;

	UTF16::Unit operator[]( size_t pos ) const;
	size_t Read( size_t start, size_t count, UTF16::Unit* dest ) const;

	TextChange Insert( size_t pos, UTF16Ref );
	TextChange Delete( size_t pos, size_t count );

	void CopyToClipboard( size_t start, size_t count ) const;

	size_t NextCharStop( size_t pos ) const { return NextBreak( m_charIter, pos ); }
	size_t NextWordStop( size_t pos ) const { return NextBreak( m_wordIter, pos ); }
	size_t PrevCharStop( size_t pos ) const { return PrevBreak( m_charIter, pos ); }
	size_t PrevWordStop( size_t pos ) const { return PrevBreak( m_wordIter, pos ); }

	size_t NextNonWhitespace( size_t pos ) const;
	size_t PrevNonWhitespace( size_t pos ) const;

	//TextChange Undo();
	//TextChange Redo();

	//bool CanUndo() const;
	//bool CanRedo() const;

	//void BeginUndoGroup();
	//void EndUndoGroup();

private:
	size_t SizeWithCRLF( size_t start, size_t count ) const;
	void   ReadWithCRLF( size_t start, size_t count, ArrayOf<UTF16::Unit> out ) const;

	void ResetIterators() const;
	size_t NextBreak( icu::BreakIterator*, size_t ) const;
	size_t PrevBreak( icu::BreakIterator*, size_t ) const;

	GapBuffer<UTF16::Unit> m_buffer;

	mutable bool m_needIterReset;
	UErrorCode m_charErrorStatus;
	UErrorCode m_wordErrorStatus;
	icu::BreakIterator* m_charIter;
	icu::BreakIterator* m_wordIter;
};

inline size_t TextDocument::Length() const
{
	return m_buffer.size();
}

inline UTF16::Unit TextDocument::operator[]( size_t pos ) const
{
	return m_buffer[pos];
}

inline size_t TextDocument::Read( size_t start, size_t count, UTF16::Unit* dest ) const
{
	return m_buffer.copy( dest, count, start );
}

#endif
