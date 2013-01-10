// Document.cpp

#include "Document.h"

Document::Document()
	: m_lineBuffer( m_charBuffer )
	, m_undoBuffer( m_charBuffer )
{
}

static const wchar_t* FindLineBreak( const wchar_t* it, const wchar_t* end )
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

CharChange Document::Insert( size_t pos, ArrayRef<const wchar_t> text )
{
	CharChange change;

	if ( text.empty() )
		return change;

	// This normalizes the line endings to 0x0A while inserting

	for ( const wchar_t* it = text.begin(); it != text.end(); )
	{
		const wchar_t* lineBreak = FindLineBreak( it, text.end() );

		if ( lineBreak != it )
			change += m_charBuffer.Insert( pos + change.delta, ArrayRef<const wchar_t>( it, lineBreak - it ) );

		if ( lineBreak != text.end() )
			change += m_charBuffer.Insert( pos + change.delta, 0x0A );

		it = SkipLineBreak( lineBreak, text.end() );
	}

	return change;
}

CharChange Document::Delete( size_t pos, size_t count )
{
	CharChange change;

	if ( count == 0 )
		return change;

	change += m_charBuffer.Delete( pos, count );
	return change;
}

ArrayRef<const wchar_t> Document::Read( size_t pos, size_t count ) const
{
	return m_charBuffer.Read( pos, count );
}

UndoChange Document::Undo()
{
	UndoChange change;
	return change;
}

CharChange Document::Redo()
{
	CharChange change;
	return change;
}
