// VisualPainter.cpp

#include "VisualPainter.h"
#include "TextStyleRegistry.h"
#include "TextStyle.h"
#include "TextSquiggle.h"

#undef min
#undef max

VisualPainter::VisualPainter( HDC hdc, const TextDocument& doc, TextStyleRegistry& styleRegistry, const TextSquiggle& squiggle, TextSelection selection )
	: hdc( hdc )
	, styleRegistry( styleRegistry )
	, doc( doc )
	, selection( selection )
	, oldSelection( selection )
	, textStart( 0 )
	, docReader( doc )
	, styleReader( styleRegistry )
	, squiggle( squiggle )
{
	GetWindowOrgEx( hdc, &oldOrigin );

	const TextStyle& defaultStyle = styleRegistry.Style( styleRegistry.defaultStyleid );
	const TextFont& defaultFont = styleRegistry.Font( defaultStyle.fontid );

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

	selection.start = oldSelection.start - std::min( oldSelection.start, textStart );
	selection.end   = oldSelection.end - std::min( oldSelection.end, textStart );

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
		::SetTextColor( hdc, styleRegistry.defaultTextColor );
	else
		::SetTextColor( hdc, color );
}

void VisualPainter::DrawSquiggles( int xStart, int xEnd, RECT rect )
{
	squiggle.Draw( hdc, xStart, xEnd, rect.top + styleRegistry.lineHeight );
}
