// DocumentReader.h

#ifndef DocumentReader_h
#define DocumentReader_h

#include "UString.h"
#include <vector>

class TextDocument;

class DocumentReader
{
public:
	DocumentReader( const TextDocument& );
	DocumentReader( const TextDocument&, size_t paragraphStart, size_t paragraphCount );

	UTF16Ref WeakRange  ( size_t textStart, size_t textCount );
	UTF16Ref StrictRange( size_t textStart, size_t textCount );

private:
	const TextDocument& m_doc;
	size_t m_paragraphStart;
	size_t m_paragraphCount;

	std::vector<UTF16::Unit> m_text;
};

#endif
