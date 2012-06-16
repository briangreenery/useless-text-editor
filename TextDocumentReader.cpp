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
	m_text[unitsRead] = UTF16::Unit( 0 );

	return UTF16Ref( &m_text.front(), &m_text.front() + unitsRead );
}

UTF16Ref TextDocumentReader::StrictRange( size_t textStart, size_t textCount )
{
	UTF16Ref result = WeakRange( textStart, textCount );
	assert( result.size() == textCount );
	return result;
}
