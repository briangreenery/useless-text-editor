// DocumentIterator.cpp

#include "DocumentIterator.h"

DocumentIterator::DocumentIterator( const TextDocument& doc )
	: m_reader( doc )
	, m_offset( 0 )
{
}

void DocumentIterator::SetOffset( size_t offset )
{
	m_offset = offset;
}

UTF16Ref DocumentIterator::Next( size_t count )
{
	return m_reader.WeakRange( m_offset, count );
}
