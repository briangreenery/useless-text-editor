// UniscribeLayout.h

#ifndef UniscribeLayout_h
#define UniscribeLayout_h

#include "TextBlock.h"
#include "UString.h"
#include <Windows.h>

class TextDocument;
class TextStyle;

TextBlockPtr UniscribeLayoutParagraph( UTF16Ref,
                                       const TextDocument&,
                                       TextStyle&,
                                       HDC,
                                       int maxWidth,
                                       bool endsWithNewline );

#endif
