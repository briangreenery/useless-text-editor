// TextLayoutArgs.h

#ifndef TextLayoutArgs_h
#define TextLayoutArgs_h

#include "ArrayRef.h"
#include <Windows.h>

class CharBuffer;
class TextStyleRegistry;
class TextFontRun;

class TextLayoutArgs
{
public:
	TextLayoutArgs( const CharBuffer&, TextStyleRegistry&, HDC, int maxWidth );

	ArrayRef<const wchar_t> text;
	ArrayRef<const TextFontRun> fonts;
	bool endsWithNewline;
	size_t textStart;
	const CharBuffer& charBuffer;
	TextStyleRegistry& styleRegistry;
	HDC hdc;
	int maxWidth;
};

#endif
