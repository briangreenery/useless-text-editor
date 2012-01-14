// TextDocument.cpp

#include "TextDocument.h"
#include "DocumentCharIter.h"
#include "Assert.h"
#include <Windows.h>

#undef min
#undef max

TextDocument::TextDocument()
	: m_charErrorStatus( U_ZERO_ERROR )
	, m_wordErrorStatus( U_ZERO_ERROR )
	, m_charIter( icu::BreakIterator::createCharacterInstance( icu::Locale::getUS(), m_charErrorStatus ) )
	, m_wordIter( icu::BreakIterator::createWordInstance     ( icu::Locale::getUS(), m_wordErrorStatus ) )
	, m_lineIter( icu::BreakIterator::createLineInstance( icu::Locale::getDefault(), m_lineErrorStatus ) )
	, m_needIterReset( true )
{
}

TextDocument::~TextDocument()
{
	// TODO: Figure out why this uses ICU's allocator.
	delete m_charIter;
	delete m_wordIter;
}

size_t TextDocument::SizeWithCRLF( size_t start, size_t count ) const
{
	return count + m_buffer.count( start, count, 0x0A );
}

void TextDocument::ReadWithCRLF( size_t start, size_t count, ArrayOf<UTF16::Unit> out ) const
{
	size_t numCopied = m_buffer.copy( out.begin(), count, start );

	UTF16::Unit* read  = out.begin() + numCopied - 1;
	UTF16::Unit* write = out.end() - 1;
	UTF16::Unit* rend  = out.begin() - 1;

	for ( ; rend != read && read != write; --read )
	{
		UTF16::Unit unit = *read;

		*write-- = unit;

		if ( unit == 0x0A )
			*write-- = 0x0D;
	}
}

static const UTF16::Unit* NextLineBreak( const UTF16::Unit* it, const UTF16::Unit* end )
{
	for ( ; it != end; ++it )
		if ( *it == 0x0A || *it == 0x0D )
			return it;

	return end;
}

static const UTF16::Unit* SkipLineBreak( const UTF16::Unit* lineBreak, const UTF16::Unit* end )
{
	if ( lineBreak == end )
		return end;

	if ( *lineBreak != 0x0D )
		return lineBreak + 1;

	++lineBreak;

	if ( lineBreak != end && *lineBreak == 0x0A )
		return lineBreak + 1;

	return lineBreak;
}

TextChange TextDocument::Insert( size_t pos, UTF16Ref text )
{
	if ( text.empty() )
		return TextChange();

	// TODO: validate unicode (?)

	// Normalize line endings to U+000A (Line Feed)

	size_t count = 0;

	for ( const UTF16::Unit* it = text.begin(); it != text.end(); )
	{
		const UTF16::Unit* lineBreak = NextLineBreak( it, text.end() );

		if ( lineBreak != it )
		{
			m_buffer.insert( pos + count, it, lineBreak - it );
			count += lineBreak - it;
		}

		if ( lineBreak != text.end() )
		{
			m_buffer.insert( pos + count, 0x0A );
			count++;
		}

		it = SkipLineBreak( lineBreak, text.end() );
	}

	m_undo.RecordInsertion( *this, pos, count );

	m_needIterReset = true;
	return TextChange( pos, count, TextChange::insertion );
}

TextChange TextDocument::Delete( size_t pos, size_t count )
{
	if ( count == 0 )
		return TextChange();

	m_undo.RecordDeletion( *this, pos, count );

	m_buffer.erase( pos, count );

	m_needIterReset = true;
	return TextChange( pos, count, TextChange::deletion );
}

std::pair<TextChange,TextSelection> TextDocument::Undo()
{
	return m_undo.Undo( *this );
}

TextChange TextDocument::Redo()
{
	return m_undo.Redo( *this );
}

bool TextDocument::CanUndo() const
{
	return m_undo.CanUndo();
}

bool TextDocument::CanRedo() const
{
	return m_undo.CanRedo();
}

void TextDocument::SetBeforeSelection( const TextSelection& selection )
{
	m_undo.SetBeforeSelection( selection );
}

void TextDocument::EndUndoGroup()
{
	m_undo.EndGroup();
}

void TextDocument::ResetIterators() const
{
	if ( m_needIterReset )
	{
		m_needIterReset = false;
		m_charIter->adoptText( new DocumentCharIter( *this ) );
		m_wordIter->adoptText( new DocumentCharIter( *this ) );
		m_lineIter->adoptText( new DocumentCharIter( *this ) );
	}
}

size_t TextDocument::NextBreak( icu::BreakIterator* iter, size_t pos ) const
{
	ResetIterators();
	int32_t result = iter->following( pos );
	return ( result == icu::BreakIterator::DONE ) ? pos : result;
}

size_t TextDocument::PrevBreak( icu::BreakIterator* iter, size_t pos ) const
{
	ResetIterators();
	int32_t result = iter->preceding( pos );
	return ( result == icu::BreakIterator::DONE ) ? pos : result;
}

static bool IsWhitespace( UTF16::Unit unit )
{
	return unit == ' ' || unit == '\t' || unit == '\n' || unit == '\r';
}

size_t TextDocument::NextNonWhitespace( size_t pos ) const
{
	for ( ; pos < Length(); ++pos )
		if ( !IsWhitespace( m_buffer[pos] ) )
			break;

	return pos;
}

size_t TextDocument::PrevNonWhitespace( size_t pos ) const
{
	for ( ; pos != 0; --pos )
		if ( !IsWhitespace( m_buffer[pos - 1] ) )
			break;

	return pos;
}

std::pair<size_t, size_t> TextDocument::WordAt( size_t pos ) const
{
	std::pair<size_t, size_t> word( pos, pos );

	bool leftSpace  = ( pos > 0 )        && IsWhitespace( m_buffer[pos - 1] );
	bool rightSpace = ( pos < Length() ) && IsWhitespace( m_buffer[pos] );

	if ( leftSpace && rightSpace )
	{
		word.first  = PrevNonWhitespace( pos );
		word.second = NextNonWhitespace( pos );
	}
	else if ( leftSpace && !rightSpace )
	{
		word.second = NextWordStop( pos );
	}
	else if ( !leftSpace && rightSpace )
	{
		word.first = PrevWordStop( pos );
	}
	else
	{
		ResetIterators();

		if ( pos == Length() || !m_wordIter->isBoundary( pos ) )
			word.first = PrevWordStop( pos );

		word.second = NextWordStop( pos );
	}

	return word;
}
