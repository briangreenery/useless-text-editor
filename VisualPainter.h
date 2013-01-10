// VisualPainter.h

#ifndef VisualPainter_h
#define VisualPainter_h

#include "TextSelection.h"
#include "TextStyleReader.h"
#include <stdint.h>
#include <Windows.h>
#include <usp10.h>

class TextSquiggle;
class TextStyleRegistry;
class Document;

class VisualPainter
{
public:
	VisualPainter( HDC, const Document&, TextStyleRegistry&, const TextSquiggle&, TextSelection );
	~VisualPainter();

	void SetOrigin( size_t textStart, LONG yStart );
	void SetTextColor( COLORREF );

	void FillRect ( RECT rect, COLORREF color );
	void DrawSquiggles( int xStart, int xEnd, RECT rect );
	void DrawHighlight( int xStart, int xEnd, RECT rect );

	HDC                 hdc;
	TextSelection       selection;
	const Document&     doc;
	TextStyleRegistry&  styleRegistry;
	size_t              textStart;
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
