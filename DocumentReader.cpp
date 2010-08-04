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

UTF16Ref DocumentReader::WeakRange( size_t textStart, size_t textCount ) const
{
	ArrayOf<UTF16::Unit> buffer = m_cache.TempArray( textCount + 1 );

	size_t unitsRead = m_doc.Read( m_paragraphStart + textStart, textCount, buffer.begin() );

	// Null termination makes it easier to look at things in the debugger
	buffer[unitsRead] = UTF16::Unit( 0 );
	return UTF16Ref( buffer.begin(), unitsRead );
}

UTF16Ref DocumentReader::StrictRange( size_t textStart, size_t textCount ) const
{
	Assert( textStart + textCount <= m_paragraphCount );
	UTF16Ref result = WeakRange( textStart, textCount );
	Assert( result.size() == textCount );
	return result;
}
