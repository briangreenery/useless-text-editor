// VisualPainter.h

#ifndef VisualPainter_h
#define VisualPainter_h

#include "TextSelection.h"
#include "TextDocumentReader.h"
#include "TextStyleReader.h"
#include "Integers.h"
#include <Windows.h>
#include <usp10.h>

class TextStyleRegistry;
class TextDocument;

class VisualPainter
{
public:
	VisualPainter( HDC, const TextDocument&, TextStyleRegistry&, TextSelection );
	~VisualPainter();

	void SetOrigin( size_t textStart, LONG yStart );
	void SetTextColor( COLORREF );
	void FillRect( RECT rect, COLORREF color );

	HDC                 hdc;
	TextSelection       selection;
	const TextDocument& doc;
	TextStyleRegistry&  styleRegistry;
	size_t              textStart;
	TextDocumentReader  docReader;
	TextStyleReader     styleReader;

private:
	TextSelection oldSelection;
	POINT         oldOrigin;
	HGDIOBJ       oldFont;
	COLORREF      oldTextColor;
	COLORREF      oldBkColor;
	int           oldBkMode;
};

#endif
