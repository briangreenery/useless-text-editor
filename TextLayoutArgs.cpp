// TextLayoutArgs.cpp

#include "TextLayoutArgs.h"

TextLayoutArgs::TextLayoutArgs( const CharBuffer& charBuffer,
                                TextStyleRegistry& styleRegistry,
                                HDC hdc,
                                int maxWidth )
	: endsWithNewline( false )
	, textStart( 0 )
	, charBuffer( charBuffer )
	, styleRegistry( styleRegistry )
	, hdc( hdc )
	, maxWidth( maxWidth )
{
}
