// DocumentReader.cpp

#include "DocumentReader.h"
#include "TextDocument.h"
#include "Assert.h"
#include <limits>

#undef min
#undef max

DocumentReader::DocumentReader( const TextDocument& doc )
	: m_doc( doc )
	, m_paragraphStart( 0 )
	, m_paragraphCount( std::numeric_limits<size_t>::max() )
{
}

DocumentReader::DocumentReader( const TextDocument& doc, size_t paragraphStart, size_t paragraphCount )
	: m_doc( doc )
	, m_paragraphStart( paragraphStart )
	, m_paragraphCount( paragraphCount )
{
}

UTF16Ref DocumentReader::WeakRange( size_t textStart, size_t textCount )
{
	m_text.resize( textCount + 1 );

	size_t unitsRead = m_doc.Read( m_paragraphStart + textStart, textCount, &m_text.front() );

	m_text[unitsRead] = UTF16::Unit( 0 );
	return UTF16Ref( &m_text.front(), &m_text.front() + unitsRead );
}

UTF16Ref DocumentReader::StrictRange( size_t textStart, size_t textCount )
{
	Assert( textStart + textCount <= m_paragraphCount );
	UTF16Ref result = WeakRange( textStart, textCount );
	Assert( result.size() == textCount );
	return result;
}
