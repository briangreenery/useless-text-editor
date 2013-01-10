// VisualPainter.cpp

#include "VisualPainter.h"
#include "TextStyleRegistry.h"
#include "TextStyle.h"
#include "TextSquiggle.h"
#include <Windows.h>
#include <GdiPlus.h>

VisualPainter::VisualPainter( HDC hdc, const CharBuffer& charBuffer, TextStyleRegistry& styleRegistry, const TextSquiggle& squiggle, CharSelection selection )
	: hdc( hdc )
	, styleRegistry( styleRegistry )
	, charBuffer( charBuffer )
	, selection( selection )
	, oldSelection( selection )
	, textStart( 0 )
	, styleReader( styleRegistry )
	, squiggle( squiggle )
{
	GetWindowOrgEx( hdc, &oldOrigin );

	const TextStyle& defaultStyle = styleRegistry.DefaultStyle();
	const TextFont& defaultFont = styleRegistry.DefaultFont();

	oldFont = SelectObject( hdc, defaultFont.hfont );
	oldTextColor = ::SetTextColor( hdc, defaultStyle.textColor );
	oldBkColor = SetBkColor( hdc, defaultStyle.bkColor );
	oldBkMode = SetBkMode( hdc, TRANSPARENT );
}

VisualPainter::~VisualPainter()
{
	SetBkMode( hdc, oldBkMode );
	SetBkColor( hdc, oldBkColor );
	::SetTextColor( hdc, oldTextColor );
	SelectObject( hdc, oldFont );

	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y, NULL );
}

void VisualPainter::SetOrigin( size_t _textStart, LONG yStart )
{
	textStart = _textStart;

	selection.start = oldSelection.start - (std::min)( oldSelection.start, textStart );
	selection.end   = oldSelection.end - (std::min)( oldSelection.end, textStart );

	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y - yStart, NULL );
}

void VisualPainter::FillRect( RECT rect, COLORREF color )
{
	SetBkColor( hdc, color );
	ExtTextOutW( hdc, rect.left, rect.top, ETO_OPAQUE|ETO_CLIPPED, &rect, L"", 0, NULL );
}

void VisualPainter::SetTextColor( COLORREF color )
{
	if ( color == TextStyle::useDefault )
		::SetTextColor( hdc, styleRegistry.DefaultStyle().textColor );
	else
		::SetTextColor( hdc, color );
}

void VisualPainter::DrawSquiggles( int xStart, int xEnd, RECT rect )
{
	squiggle.Draw( hdc, xStart, xEnd, rect.top + styleRegistry.LineHeight() );
}

static void RoundRect( HDC hdc, int x, int y, int width, int height, int radius, COLORREF topColorRef, COLORREF bottomColorRef )
{
	using namespace Gdiplus;

	GraphicsPath path;

	path.AddLine(x + radius, y, x + width - (radius * 2), y);
    path.AddArc(x + width - (radius * 2), y, radius * 2, radius * 2, 270, 90);
    path.AddLine(x + width, y + radius, x + width, y + height - (radius * 2));
    path.AddArc(x + width - (radius * 2), y + height - (radius * 2), radius * 2, radius * 2, 0, 90);
    path.AddLine(x + width - (radius * 2), y + height, x + radius, y + height);
    path.AddArc(x, y + height - (radius * 2), radius * 2, radius * 2, 90, 90);
    path.AddLine(x, y + height - (radius * 2), x, y + radius);
    path.AddArc(x, y, radius * 2, radius * 2, 180, 90);
    path.CloseFigure();

	Color topColor;
	topColor.SetFromCOLORREF( topColorRef );

	Color bottomColor;
	bottomColor.SetFromCOLORREF( bottomColorRef );

	Rect gradientRect( x, y, width, height );
	LinearGradientBrush brush( gradientRect, topColor, bottomColor, LinearGradientModeVertical );

	Graphics g( hdc );
	g.SetSmoothingMode( SmoothingModeHighQuality );
	g.FillPath( &brush, &path );
}

void VisualPainter::DrawHighlight( int xStart, int xEnd, RECT rect )
{
	RoundRect( hdc, xStart,     rect.top + 1, xEnd - xStart,     styleRegistry.LineHeight(), 3, RGB( 190, 190, 190 ), RGB( 190, 190, 190 ) );
	RoundRect( hdc, xStart - 1, rect.top    , xEnd - xStart + 1, styleRegistry.LineHeight(), 3, RGB( 255, 215,  87 ), RGB( 255, 188,  43 ) );
}
