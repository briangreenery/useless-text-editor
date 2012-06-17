// TextDocument.h

#ifndef TextDocument_h
#define TextDocument_h

#include "TextChange.h"
#include "TextBuffer.h"
#include "UTF16Ref.h"
#include "TextDocumentUndo.h"
#include "ArrayRef.h"
#include <unicode/brkiter.h>
#include <utility>
#include <vector>

class TextDocument
{
public:
	TextDocument();
	~TextDocument();

	size_t Length() const;
	bool Empty() const;

	wchar_t operator[]( size_t pos ) const;
	size_t Read( size_t start, size_t count, wchar_t* dest ) const;

	TextChange Insert( size_t pos, UTF16Ref );
	TextChange Delete( size_t pos, size_t count );

	size_t NextCharStop( size_t pos ) const { return NextBreak( m_charIter, pos ); }
	size_t PrevCharStop( size_t pos ) const { return PrevBreak( m_charIter, pos ); }

	size_t NextWordStop( size_t pos ) const { return NextBreak( m_wordIter, pos ); }
	size_t PrevWordStop( size_t pos ) const { return PrevBreak( m_wordIter, pos ); }

	size_t NextSoftBreak( size_t pos ) const { return NextBreak( m_lineIter, pos ); }
	size_t PrevSoftBreak( size_t pos ) const { return PrevBreak( m_lineIter, pos ); }

	size_t NextNonWhitespace( size_t pos ) const;
	size_t PrevNonWhitespace( size_t pos ) const;

	std::pair<size_t, size_t> WordAt( size_t pos ) const;

	size_t SizeWithCRLF( size_t start, size_t count ) const;
	void ReadWithCRLF( size_t start, size_t count, ArrayRef<wchar_t> out ) const;

	std::pair<TextChange,TextSelection> Undo();
	TextChange                          Redo();

	bool CanUndo() const;
	bool CanRedo() const;

	void SetBeforeSelection( const TextSelection& );
	void EndUndoGroup();

	size_t LineCount() const;

private:
	void ResetIterators() const;
	size_t NextBreak( icu::BreakIterator*, size_t ) const;
	size_t PrevBreak( icu::BreakIterator*, size_t ) const;

	TextBuffer m_buffer;
	TextDocumentUndo m_undo;
	size_t m_lineCount;

	mutable bool m_needIterReset;
	UErrorCode m_charErrorStatus;
	UErrorCode m_wordErrorStatus;
	UErrorCode m_lineErrorStatus;
	icu::BreakIterator* m_charIter;
	icu::BreakIterator* m_wordIter;
	icu::BreakIterator* m_lineIter;
};

inline size_t TextDocument::Length() const
{
	return m_buffer.size();
}

inline bool TextDocument::Empty() const
{
	return m_buffer.empty();
}

inline wchar_t TextDocument::operator[]( size_t pos ) const
{
	return m_buffer[pos];
}

inline size_t TextDocument::Read( size_t start, size_t count, wchar_t* dest ) const
{
	return m_buffer.copy( dest, count, start );
}

#endif
