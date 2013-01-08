// Document.cpp

#include "Document.h"

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

CharChange Document::Insert( size_t pos, const wchar_t* text, size_t count )
{
	CharChange change;

	if ( count == 0 )
		return change;

	// This normalizes the line endings to 0x0A while inserting

	for ( const wchar_t* it = text; it != text + count; )
	{
		const wchar_t* lineBreak = NextLineBreak( it, text + count );

		if ( lineBreak != it )
			change += m_charBuffer.Insert( pos, it, lineBreak - it );

		if ( lineBreak != text + count )
			change += m_charBuffer.Insert( pos + count, 0x0A );

		it = SkipLineBreak( lineBreak, text + count );
	}

	m_undoBuffer.Update( m_charBuffer, change );
	m_lineBuffer.Update( m_charBuffer, change );

	return change;
}

CharChange Document::Delete( size_t pos, size_t count )
{
	CharChange change;

	if ( count == 0 )
		return change;

	change += m_charBuffer.Delete( pos, count );

	m_undoBuffer.Update( m_charBuffer, change );
	m_lineBuffer.Update( m_charBuffer, change );

	return change;
}

UndoChange Document::Undo()
{
	UndoChange change = m_undoBuffer.Undo();

	m_lineBuffer.Update( m_charBuffer, change.charChange );

	return change;
}

CharChange Document::Redo()
{
	CharChange change = m_undoBuffer.Redo();

	m_lineBuffer.Update( m_charBuffer, change );

	return change;
}
