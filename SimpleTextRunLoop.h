// SimpleTextRunLoop.h

#ifndef SimpleTextRunLoop_h
#define SimpleTextRunLoop_h

#include "SimpleTextRun.h"
#include "TextFontRun.h"
#include "UString.h"
#include "ArrayOf.h"
#include <vector>

class SimpleTextRunLoop
{
public:
	SimpleTextRunLoop( UTF16Ref text, ArrayOf<const TextFontRun> );
	
	bool Unfinished() const;
	SimpleTextRun NextRun();
	void NewLine( size_t start );

private:
	UTF16Ref m_text;

	size_t m_position;
	size_t m_nextTab;

	ArrayOf<const TextFontRun> m_fonts;
	const TextFontRun*         m_font;
	size_t                     m_fontStart;
};

#endif
