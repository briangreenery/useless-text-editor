// UniscribeLayout.h

#ifndef UniscribeLayout_h
#define UniscribeLayout_h

#include "TextBlock.h"
#include "UString.h"
#include <Windows.h>

class TextStyle;

TextBlockPtr UniscribeLayoutParagraph( UTF16Ref, TextStyle&, HDC, int maxWidth, bool endsWithNewline );

#endif
