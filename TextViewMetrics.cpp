// TextViewMetrics.cpp

#include "TextViewMetrics.h"
#include <algorithm>

#undef min
#undef max

TextViewMetrics::TextViewMetrics()
	: caretWidth( GetSystemMetrics( SM_CXBORDER ) )
	, marginWidth( 0 )
	, gutterWidth( 0 )
	, lineHeight( 0 )
	, linesPerPage( 0 )
	, xOffset( 0 )
	, yOffset( 0 )
{
}

RECT TextViewMetrics::GutterRect( HWND hwnd ) const
{
	RECT rect;
	GetClientRect( hwnd, &rect );

	rect.right = gutterWidth;
	return rect;
}

RECT TextViewMetrics::TextRect( HWND hwnd ) const
{
	RECT rect;
	GetClientRect( hwnd, &rect );

	rect.left += gutterWidth;
	InflateRect( &rect, -marginWidth, -marginWidth );
	return rect;
}

RECT TextViewMetrics::TextOrMarginRect( HWND hwnd ) const
{
	RECT rect;
	GetClientRect( hwnd, &rect );

	rect.left += gutterWidth;
	return rect;
}

RECT TextViewMetrics::IntersectWithGutter( RECT rect, HWND hwnd ) const
{
	RECT gutter = GutterRect( hwnd );
	RECT intersection;
	IntersectRect( &intersection, &rect, &gutter );
	return intersection;
}

RECT TextViewMetrics::IntersectWithText( RECT rect, HWND hwnd ) const
{
	RECT text = TextRect( hwnd );
	RECT intersection;
	IntersectRect( &intersection, &rect, &text );
	return intersection;
}

RECT TextViewMetrics::IntersectWithTextOrMargin( RECT rect, HWND hwnd ) const
{
	RECT text = TextOrMarginRect( hwnd );
	RECT intersection;
	IntersectRect( &intersection, &rect, &text );
	return intersection;
}

bool TextViewMetrics::IsInGutter( POINT point, HWND hwnd ) const
{
	RECT gutter = GutterRect( hwnd );
	return PtInRect( &gutter, point ) ? true : false;
}

bool TextViewMetrics::IsInText( POINT point, HWND hwnd ) const
{
	RECT text = TextRect( hwnd );
	return PtInRect( &text, point ) ? true : false;
}

bool TextViewMetrics::IsInTextOrMargin( POINT point, HWND hwnd ) const
{
	RECT text = TextOrMarginRect( hwnd );
	return PtInRect( &text, point ) ? true : false;
}

HDC TextViewMetrics::ClientToGutter( HDC hdc ) const
{
	POINT clientOrigin = { 0, 0 };
	clientOrigin = ClientToGutter( clientOrigin );

	SetWindowOrgEx( hdc, clientOrigin.x, clientOrigin.y, NULL );
	return hdc;
}

RECT TextViewMetrics::ClientToGutter( RECT rect ) const
{
	POINT topLeft     = { rect.left, rect.top };
	POINT bottomRight = { rect.right, rect.bottom };

	topLeft     = ClientToGutter( topLeft );
	bottomRight = ClientToGutter( bottomRight );

	RECT result = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
	return result;
}

POINT TextViewMetrics::ClientToGutter( POINT pt ) const
{
	pt.y += -yOffset;
	return pt;
}

POINT TextViewMetrics::GutterToClient( POINT pt ) const
{
	pt.y += -yOffset;
	return pt;
}

HDC TextViewMetrics::ClientToText( HDC hdc, POINT* oldOrigin ) const
{
	POINT clientOrigin = { 0, 0 };
	clientOrigin = ClientToText( clientOrigin );

	SetWindowOrgEx( hdc, clientOrigin.x, clientOrigin.y, oldOrigin );
	return hdc;
}

RECT TextViewMetrics::ClientToText( RECT rect ) const
{
	POINT topLeft     = { rect.left, rect.top };
	POINT bottomRight = { rect.right, rect.bottom };

	topLeft     = ClientToText( topLeft );
	bottomRight = ClientToText( bottomRight );

	RECT result = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
	return result;
}

POINT TextViewMetrics::ClientToText( POINT pt ) const
{
	pt.y -= marginWidth - yOffset;
	pt.x -= marginWidth + gutterWidth - xOffset;
	return pt;
}

POINT TextViewMetrics::TextToClient( POINT pt ) const
{
	pt.y += marginWidth - yOffset;
	pt.x += marginWidth + gutterWidth - xOffset;
	return pt;
}
