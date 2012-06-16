// TextLayoutArgs.h

#ifndef TextLayoutArgs_h
#define TextLayoutArgs_h

#include "UString.h"
#include "ArrayRef.h"
#include <Windows.h>

class TextDocument;
class TextStyleRegistry;
class TextFontRun;

class TextLayoutArgs
{
public:
	TextLayoutArgs( const TextDocument&, TextStyleRegistry&, HDC, int maxWidth );

	UTF16Ref text;
	ArrayRef<const TextFontRun> fonts;
	bool endsWithNewline;
	size_t textStart;
	const TextDocument& doc;
	TextStyleRegistry& styleRegistry;
	HDC hdc;
	int maxWidth;
};

#endif
