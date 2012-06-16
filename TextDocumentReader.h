// TextDocumentReader.h

#ifndef TextDocumentReader_h
#define TextDocumentReader_h

#include "UTF16Ref.h"
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
	std::vector<wchar_t> m_text;
};

#endif
