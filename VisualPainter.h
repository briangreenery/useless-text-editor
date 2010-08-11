// VisualPainter.h

#ifndef VisualPainter_h
#define VisualPainter_h

#include "TextStyle.h"
#include "TextSelection.h"
#include "VisualSelection.h"
#include <Windows.h>
#include <usp10.h>

class VisualPainter
{
public:
	VisualPainter( HDC, TextStyle&, TextSelection );
	~VisualPainter();

	void SetOrigin( size_t textStart, LONG yStart );
	void SetFont( size_t font );

	HDC              hdc;
	TextSelection    selection;
	TextStyle&       style;

private:
	TextSelection oldSelection;
	POINT oldOrigin;
	HGDIOBJ oldFont;
	int oldBkMode;
	size_t currentFont;
};

#endif
