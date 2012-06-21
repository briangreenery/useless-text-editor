// TextDocumentReader.cpp

#include "TextDocumentReader.h"
#include "TextDocument.h"
#include <cassert>

TextDocumentReader::TextDocumentReader( const TextDocument& doc )
	: m_doc( doc )
{
}

UTF16Ref TextDocumentReader::WeakRange( size_t textStart, size_t textCount )
{
	m_text.resize( textCount + 1 );

	size_t unitsRead = m_doc.Read( textStart, textCount, &m_text.front() );
	m_text[unitsRead] = wchar_t( 0 );

	return UTF16Ref( &m_text.front(), &m_text.front() + unitsRead );
}

UTF16Ref TextDocumentReader::StrictRange( size_t textStart, size_t textCount )
{
	UTF16Ref result = WeakRange( textStart, textCount );
	assert( result.size() == textCount );
	return result;
}

static bool IsAscii( wchar_t c )
{
	return 0 <= c && c < 128;
}

AsciiRef TextDocumentReader::AsciiRange( size_t textStart, size_t textCount )
{
	UTF16Ref text = WeakRange( textStart, textCount );

	m_ascii.resize( text.size() + 1 );
	
	for ( size_t i = 0; i < text.size(); ++i )
		m_ascii[i] = IsAscii( text[i] ) ? text[i] : 0xff;

	m_ascii[text.size()] = 0;

	return AsciiRef( &m_ascii.front(), &m_ascii.front() + text.size() );
}
