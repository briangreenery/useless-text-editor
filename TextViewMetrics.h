// TextViewMetrics.h

#ifndef TextViewMetrics_h
#define TextViewMetrics_h

#include <Windows.h>

class TextViewMetrics
{
public:
	TextViewMetrics();

	RECT GutterRect      ( HWND ) const;
	RECT TextRect        ( HWND ) const;
	RECT TextOrMarginRect( HWND ) const;

	RECT IntersectWithGutter      ( RECT, HWND ) const;
	RECT IntersectWithText        ( RECT, HWND ) const;
	RECT IntersectWithTextOrMargin( RECT, HWND ) const;

	bool IsInGutter      ( POINT, HWND ) const;
	bool IsInText        ( POINT, HWND ) const;
	bool IsInTextOrMargin( POINT, HWND ) const;

	RECT  ClientToGutter( RECT ) const;
	POINT ClientToGutter( POINT ) const;
	POINT GutterToClient( POINT ) const;

	HDC   ClientToText( HDC, POINT* ) const;
	RECT  ClientToText( RECT ) const;
	POINT ClientToText( POINT ) const;
	POINT TextToClient( POINT ) const;

	int caretWidth;

	int marginWidth;
	int gutterWidth;

	int linesPerPage;

	int xOffset;
	int yOffset;

	RECT clientRect;
};

#endif
