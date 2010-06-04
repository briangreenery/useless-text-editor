// DocumentReader.h

#ifndef DocumentReader_h
#define DocumentReader_h

#include "VectorCache.h"
#include "UString.h"

class TextDocument;

class DocumentReader
{
public:
	DocumentReader( const TextDocument& );
	DocumentReader( const TextDocument&, size_t paragraphStart, size_t paragraphCount );

	UTF16Ref WeakRange  ( size_t textStart, size_t textCount ) const;
	UTF16Ref StrictRange( size_t textStart, size_t textCount ) const;

private:
	const TextDocument& m_doc;
	size_t m_paragraphStart;
	size_t m_paragraphCount;

	mutable VectorCache<UTF16::Unit> m_cache;
};

#endif
