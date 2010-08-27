// SimpleLayout.h

#ifndef SimpleLayout_h
#define SimpleLayout_h

#include "TextBlock.h"
#include "UString.h"
#include <Windows.h>

class TextDocument;
class TextStyle;

TextBlockPtr SimpleLayoutParagraph( UTF16Ref,
                                    const TextDocument&,
                                    TextStyle&,
                                    HDC,
                                    int maxWidth,
                                    bool endsWithNewline );

#endif
