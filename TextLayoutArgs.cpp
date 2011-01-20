// TextLayoutArgs.cpp

#include "TextLayoutArgs.h"

TextLayoutArgs::TextLayoutArgs( const TextDocument& _doc,
                                TextStyleRegistry& _styleRegistry,
                                HDC _hdc,
                                int _maxWidth )
	: endsWithNewline( false )
	, textStart( 0 )
	, doc( _doc )
	, styleRegistry( _styleRegistry )
	, hdc( _hdc )
	, maxWidth( _maxWidth )
{
}
