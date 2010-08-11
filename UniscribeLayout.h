// UniscribeLayout.h

#ifndef UniscribeLayout_h
#define UniscribeLayout_h

#include "UniscribeAllocator.h"
#include "UniscribeData.h"
#include "UniscribeLine.h"
#include "UString.h"
#include <Windows.h>
#include <vector>

class TextStyle;

std::vector<UniscribeLine> UniscribeLayoutParagraph( UTF16Ref, TextStyle&, HDC, int maxWidth );

#endif
