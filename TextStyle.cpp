// TextStyle.cpp

#include "TextStyle.h"

TextStyle::TextStyle()
	: fontid( useDefault )
	, textColor( useDefault )
	, bkColor( useDefault )
{
}

TextStyle::TextStyle( uint32 _fontid, COLORREF _textColor, COLORREF _bkColor )
	: fontid( _fontid )
	, textColor( _textColor )
	, bkColor( _bkColor )
{
}
