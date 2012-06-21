// TextDocumentReader.h

#ifndef TextDocumentReader_h
#define TextDocumentReader_h

#include "UTF16Ref.h"
#include <vector>

class TextDocument;

typedef ArrayRef<const char> AsciiRef;

class TextDocumentReader
{
public:
	TextDocumentReader( const TextDocument& );
	TextDocumentReader( const TextDocument&, size_t paragraphStart, size_t paragraphCount );

	UTF16Ref WeakRange  ( size_t textStart, size_t textCount );
	UTF16Ref StrictRange( size_t textStart, size_t textCount );

	AsciiRef AsciiRange( size_t textStart, size_t textCount );

private:
	const TextDocument& m_doc;
	std::vector<wchar_t> m_text;
	std::vector<char> m_ascii;
};

#endif
