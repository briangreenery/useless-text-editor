// TextDocumentReader.h

#ifndef TextDocumentReader_h
#define TextDocumentReader_h

#include "UString.h"
#include <vector>

class TextDocument;

class TextDocumentReader
{
public:
	TextDocumentReader( const TextDocument& );
	TextDocumentReader( const TextDocument&, size_t paragraphStart, size_t paragraphCount );

	UTF16Ref WeakRange  ( size_t textStart, size_t textCount );
	UTF16Ref StrictRange( size_t textStart, size_t textCount );

private:
	const TextDocument& m_doc;
	std::vector<UTF16::Unit> m_text;
};

#endif
