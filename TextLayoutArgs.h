// TextLayoutArgs.h

#ifndef TextLayoutArgs_h
#define TextLayoutArgs_h

#include "UString.h"
#include <Windows.h>

class TextDocument;
class TextStyle;

class TextLayoutArgs
{
public:
	TextLayoutArgs( const TextDocument&, TextStyle&, HDC, int maxWidth );

	UTF16Ref text;
	bool endsWithNewline;
	const TextDocument& doc;
	TextStyle& style;
	HDC hdc;
	int maxWidth;
};

#endif
