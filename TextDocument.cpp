// TextDocument.cpp

#include "TextDocument.h"
#include "Require.h"
#include <Windows.h>

#undef min
#undef max

size_t TextDocument::ReadWithCRLFSize( size_t start, size_t count ) const
{
	return count + m_buffer.count( start, count, 0x0A );
}

void TextDocument::ReadWithCRLF( size_t start, size_t count, ArrayOf<UTF16::Unit> out ) const
{
	Require( out.size() >= count );
	size_t numCopied = m_buffer.copy( out.begin(), count, start );
	Require( numCopied == count );

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

void TextDocument::CopyToClipboard( size_t start, size_t count ) const
{
	if ( !OpenClipboard( NULL ) )
		return;

	EmptyClipboard();

	size_t sizeWithCRLF = ReadWithCRLFSize( start, count );
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE, ( sizeWithCRLF + 1 ) * sizeof( UTF16::Unit ) );

	if ( hGlobal != 0 )
	{
		UTF16::Unit* dest = static_cast<UTF16::Unit*>( GlobalLock( hGlobal ) );

		if ( dest != 0 )
		{
			try
			{
				ReadWithCRLF( start, count, ArrayOf<UTF16::Unit>( dest, dest + sizeWithCRLF ) );
				dest[sizeWithCRLF] = UTF16::Unit( 0 );
			}
			catch (...)
			{
			}

			GlobalUnlock( hGlobal );

			if ( !SetClipboardData( CF_UNICODETEXT, hGlobal ) )
				GlobalFree( hGlobal );
		}
	}

	CloseClipboard();
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
		return TextChange::NoChange();

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

	return TextChange::Insertion( pos, count );
}

TextChange TextDocument::Delete( size_t pos, size_t count )
{
	if ( count == 0 )
		return TextChange::NoChange();

	m_buffer.erase( pos, count );
	return TextChange::Deletion( pos, count );
}
