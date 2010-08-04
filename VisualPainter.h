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

	HDC              hdc;
	TextSelection    selection;
	TextStyle&       style;

	VisualSelection  prevSelection;

private:
	TextSelection oldSelection;
	POINT oldOrigin;
	HGDIOBJ oldFont;
	HGDIOBJ oldPen;
	int oldBkMode;
};

#endif
