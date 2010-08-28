// TextLayoutArgs.cpp

#include "TextLayoutArgs.h"

TextLayoutArgs::TextLayoutArgs( const TextDocument& _doc,
                                TextStyle& _style,
                                HDC _hdc,
                                int _maxWidth )
	: endsWithNewline( false )
	, doc( _doc )
	, style( _style )
	, hdc( _hdc )
	, maxWidth( _maxWidth )
{
}
