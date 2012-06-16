// TextLayoutArgs.cpp

#include "TextLayoutArgs.h"

TextLayoutArgs::TextLayoutArgs( const TextDocument& doc,
                                TextStyleRegistry& styleRegistry,
                                HDC hdc,
                                int maxWidth )
	: endsWithNewline( false )
	, textStart( 0 )
	, doc( doc )
	, styleRegistry( styleRegistry )
	, hdc( hdc )
	, maxWidth( maxWidth )
{
}
