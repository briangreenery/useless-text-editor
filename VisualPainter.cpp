// VisualPainter.cpp

#include "VisualPainter.h"
#include "TextStyle.h"

#undef min
#undef max

VisualPainter::VisualPainter( HDC _hdc, const TextDocument& _doc, TextStyle& _style, TextSelection _selection )
	: hdc( _hdc )
	, style( _style )
	, doc( _doc )
	, selection( _selection )
	, oldSelection( _selection )
	, textStart( 0 )
{
	GetWindowOrgEx( hdc, &oldOrigin );

	oldFont = SelectObject( hdc, style.fonts[0].font );
	currentFont = 0;

	oldBkMode = SetBkMode( hdc, TRANSPARENT );
}

VisualPainter::~VisualPainter()
{
	SetBkMode( hdc, oldBkMode );
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

void VisualPainter::SetFont( size_t font )
{
	if ( font == currentFont )
		return;

	SelectObject( hdc, style.fonts[font].font );
	currentFont = font;
}
