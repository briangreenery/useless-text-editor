// DocumentIterator.h

#ifndef DocumentIterator_h
#define DocumentIterator_h

#include "DocumentReader.h"

class DocumentIterator
{
public:
	DocumentIterator( const TextDocument& );

	void SetOffset( size_t offset );
	UTF16Ref Next( size_t count );

private:
	DocumentReader m_reader;
	size_t m_offset;
};

#endif
