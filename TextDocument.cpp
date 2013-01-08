// TextDocument.cpp

#include "TextDocument.h"
#include "DocumentCharIter.h"
#include <cassert>
#include <Windows.h>

TextDocument::TextDocument()
	: m_charErrorStatus( U_ZERO_ERROR )
	, m_wordErrorStatus( U_ZERO_ERROR )
	, m_lineErrorStatus( U_ZERO_ERROR )
	, m_charIter( icu::BreakIterator::createCharacterInstance( icu::Locale::getUS(), m_charErrorStatus ) )
	, m_wordIter( icu::BreakIterator::createWordInstance     ( icu::Locale::getUS(), m_wordErrorStatus ) )
	, m_lineIter( icu::BreakIterator::createLineInstance( icu::Locale::getDefault(), m_lineErrorStatus ) )
	, m_needIterReset( true )
{
	m_lineLengths.Insert( 0, 0 );
}

TextDocument::~TextDocument()
{
	// Theses deletes use ICU's allocator.
	delete m_charIter;
	delete m_wordIter;
	delete m_lineIter;
}

size_t TextDocument::LineBreakCount( size_t start, size_t count ) const
{
	LineContaining_Result line = LineContaining( start );

	size_t lineBreakCount = 0;

	for ( ; line.index < m_lineLengths.Size(); ++line.index )
	{
		if ( start + count < line.textStart + m_lineLengths[line.index] )
			break;

		lineBreakCount++;
	}

	return count + lineBreakCount;
}

void TextDocument::ReadWithCRLF( size_t start, size_t count, wchar_t* outBuffer, size_t outBufferSize ) const
{
	LineContaining_Result line = LineContaining( start );

	size_t numCopied = 0;
	for ( ; line.index < m_lineBuffer.Size(); ++line.index )
	{

		numCopied += ReadLine( i, out.begin(), out.size() - numCopied );
		out.begin
	}

	size_t numCopied = m_buffer.copy( out.begin(), count, start );

	wchar_t* read  = out.begin() + numCopied - 1;
	wchar_t* write = out.end() - 1;
	wchar_t* rend  = out.begin() - 1;

	for ( ; rend != read && read != write; --read )
	{
		wchar_t unit = *read;

		*write-- = unit;

		if ( unit == 0x0A )
			*write-- = 0x0D;
	}
}

size_t TextDocument::LineLength( size_t index ) const
{
	assert( index < m_lineBuffer.Size() );
	return m_lineLengths[index];
}

size_t TextDocument::ReadLine( size_t index, wchar_t* outBuffer, size_t outBufferSize ) const
{
	assert( index < m_lineBuffer.Size() );
	assert( outBufferSize >= m_lineBuffer[index] );

	size_t textOffset = 0;
	for ( size_t i = 0; i < index; ++i )
		textOffset += m_lineLengths[i];

	return ReadText( textOffset, m_lineLengths[index], outBuffer );
}

TextDocument::LineContaining_Result TextDocument::LineContaining( size_t pos ) const
{
	LineContaining_Result line = {};

	for ( size_t lineCount = m_lineBuffer.Size(); line.index < lineCount; ++line.index )
	{
		size_t lineLength = m_lineBuffer[line.index];

		if ( pos < line.textStart + lineLength )
			return line;

		line.textStart += lineLength;
	}

	line.index--;
	line.textStart -= m_lineBuffer[line.index];
	return line;
}

size_t TextDocument::LineCount() const
{
	return m_lineCount;
}

static const wchar_t* NextLineBreak( const wchar_t* it, const wchar_t* end )
{
	for ( ; it != end; ++it )
		if ( *it == 0x0A || *it == 0x0D )
			return it;

	return end;
}

static const wchar_t* SkipLineBreak( const wchar_t* lineBreak, const wchar_t* end )
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

	// Normalize line endings to U+000A (Line Feed)

	size_t count = 0;

	for ( const wchar_t* it = text.begin(); it != text.end(); )
	{
		const wchar_t* lineBreak = NextLineBreak( it, text.end() );

		if ( lineBreak != it )
		{
			m_buffer.insert( pos + count, it, lineBreak - it );
			count += lineBreak - it;
		}

		if ( lineBreak != text.end() )
		{
			m_buffer.insert( pos + count, 0x0A );
			m_lineCount++;
			count++;
		}

		it = SkipLineBreak( lineBreak, text.end() );
	}

	assert( m_lineCount == m_buffer.count( 0, Length(), 0x0A ) + 1 );

	m_undo.RecordInsertion( *this, pos, count );

	m_needIterReset = true;
	return TextChange( pos, count, TextChange::insertion );
}

TextChange TextDocument::Delete( size_t pos, size_t count )
{
	if ( count == 0 )
		return TextChange();

	m_lineCount -= m_buffer.count( pos, count, 0x0A );

	m_undo.RecordDeletion( *this, pos, count );
	m_buffer.erase( pos, count );

	assert( m_lineCount == m_buffer.count( 0, Length(), 0x0A ) + 1 );

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

		// adoptText takes ownership of the character iterator
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

static bool IsWhitespace( wchar_t unit )
{
	return unit == ' ' || unit == '\t' || unit == '\n' || unit == '\r';
}

size_t TextDocument::NextNonWhitespace( size_t pos ) const
{
	for ( ; pos < Length(); ++pos )
		if ( !IsWhitespace( m_textBuffer[pos] ) )
			break;

	return pos;
}

size_t TextDocument::PrevNonWhitespace( size_t pos ) const
{
	for ( ; pos != 0; --pos )
		if ( !IsWhitespace( m_textBuffer[pos - 1] ) )
			break;

	return pos;
}

std::pair<size_t, size_t> TextDocument::WordAt( size_t pos ) const
{
	std::pair<size_t, size_t> word( pos, pos );

	bool leftSpace  = ( pos > 0 )        && IsWhitespace( m_textBuffer[pos - 1] );
	bool rightSpace = ( pos < Length() ) && IsWhitespace( m_textBuffer[pos] );

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
