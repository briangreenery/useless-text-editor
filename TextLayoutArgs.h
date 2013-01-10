// TextLayoutArgs.h

#ifndef TextLayoutArgs_h
#define TextLayoutArgs_h

#include "ArrayRef.h"
#include <Windows.h>

class Document;
class TextStyleRegistry;
class TextFontRun;

class TextLayoutArgs
{
public:
	TextLayoutArgs( const Document&, TextStyleRegistry&, HDC, int maxWidth );

	ArrayRef<const wchar_t> text;
	ArrayRef<const TextFontRun> fonts;
	bool endsWithNewline;
	size_t textStart;
	const Document& doc;
	TextStyleRegistry& styleRegistry;
	HDC hdc;
	int maxWidth;
};

#endif
