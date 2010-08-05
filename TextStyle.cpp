// TextStyle.cpp

#include "TextStyle.h"

TextStyle::TextStyle()
{
	LOGFONT logFont = {};
	logFont.lfHeight = -13;
	wcscpy_s( logFont.lfFaceName, L"Consolas" );
	font = CreateFontIndirect( &logFont );
	fontCache = 0;

	gutterBrush = CreateSolidBrush( RGB( 236, 236, 236 ) );
	selectionBrush = CreateSolidBrush( RGB( 173, 214, 255 ) );
	selectionHighlightPen = CreatePen( PS_SOLID, 1, RGB( 214, 235, 255 ) );

	HDC hdc = GetDC( NULL );
	SelectObject( hdc, font );
	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );
	ReleaseDC( NULL, hdc );

	lineHeight = tm.tmHeight;
	avgCharWidth = tm.tmAveCharWidth;
	tabSize = 4 * tm.tmAveCharWidth;
}
