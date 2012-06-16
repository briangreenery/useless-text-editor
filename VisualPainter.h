// VisualPainter.h

#ifndef VisualPainter_h
#define VisualPainter_h

#include "TextSelection.h"
#include "TextDocumentReader.h"
#include "TextStyleReader.h"
#include <stdint.h>
#include <Windows.h>
#include <usp10.h>

class TextSquiggle;
class TextStyleRegistry;
class TextDocument;

class VisualPainter
{
public:
	VisualPainter( HDC, const TextDocument&, TextStyleRegistry&, const TextSquiggle&, TextSelection );
	~VisualPainter();

	void SetOrigin( size_t textStart, LONG yStart );
	void SetTextColor( COLORREF );

	void FillRect ( RECT rect, COLORREF color );
	void DrawSquiggles( int xStart, int xEnd, RECT rect );
	void DrawHighlight( int xStart, int xEnd, RECT rect );

	HDC                 hdc;
	TextSelection       selection;
	const TextDocument& doc;
	TextStyleRegistry&  styleRegistry;
	size_t              textStart;
	TextDocumentReader  docReader;
	TextStyleReader     styleReader;

private:
	const TextSquiggle& squiggle;

	TextSelection oldSelection;
	POINT         oldOrigin;
	HGDIOBJ       oldFont;
	COLORREF      oldTextColor;
	COLORREF      oldBkColor;
	int           oldBkMode;
};

#endif
