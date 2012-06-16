// TextStyle.cpp

#include "TextStyle.h"

TextStyle::TextStyle()
	: fontid( useDefault )
	, textColor( useDefault )
	, bkColor( useDefault )
{
}

TextStyle::TextStyle( uint32 fontid, COLORREF textColor, COLORREF bkColor )
	: fontid( fontid )
	, textColor( textColor )
	, bkColor( bkColor )
{
}
