// VisualPainter.cpp

#include "VisualPainter.h"

#undef min
#undef max

VisualPainter::VisualPainter( HDC _hdc, TextStyle& _style, TextSelection _selection )
	: hdc( _hdc )
	, style( _style )
	, selection( _selection )
	, oldSelection( _selection )
{
	GetWindowOrgEx( hdc, &oldOrigin );

	oldFont = SelectObject( hdc, style.font );
	oldPen = SelectObject( hdc, style.selectionHighlightPen );
	oldBkMode = SetBkMode( hdc, TRANSPARENT );
}

VisualPainter::~VisualPainter()
{
	SetBkMode( hdc, oldBkMode );
	SelectObject( hdc, oldPen );
	SelectObject( hdc, oldFont );

	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y, NULL );
}

void VisualPainter::SetOrigin( size_t textStart, LONG yStart )
{
	selection.start = oldSelection.start - std::min( oldSelection.start, textStart );
	selection.end   = oldSelection.end - std::min( oldSelection.end, textStart );

	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y - yStart, NULL );
}
