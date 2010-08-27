// VisualPainter.h

#ifndef VisualPainter_h
#define VisualPainter_h

#include "TextSelection.h"
#include <Windows.h>
#include <usp10.h>

class TextStyle;
class TextDocument;

class VisualPainter
{
public:
	VisualPainter( HDC, const TextDocument&, TextStyle&, TextSelection );
	~VisualPainter();

	void SetOrigin( size_t textStart, LONG yStart );
	void SetFont( size_t font );

	HDC                 hdc;
	TextSelection       selection;
	const TextDocument& doc;
	TextStyle&          style;
	size_t              textStart;

private:
	size_t        currentFont;
	TextSelection oldSelection;
	POINT         oldOrigin;
	HGDIOBJ       oldFont;
	int           oldBkMode;
};

#endif
