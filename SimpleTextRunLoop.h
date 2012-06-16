// SimpleTextRunLoop.h

#ifndef SimpleTextRunLoop_h
#define SimpleTextRunLoop_h

#include "SimpleTextRun.h"
#include "TextFontRun.h"
#include "UTF16Ref.h"
#include "ArrayRef.h"
#include <vector>

class SimpleTextRunLoop
{
public:
	SimpleTextRunLoop( UTF16Ref text, ArrayRef<const TextFontRun> );
	
	bool Unfinished() const;
	SimpleTextRun NextRun();
	void NewLine( size_t start );

private:
	UTF16Ref m_text;

	size_t m_position;
	size_t m_nextTab;

	ArrayRef<const TextFontRun> m_fonts;
	const TextFontRun*         m_font;
	size_t                     m_fontStart;
};

#endif
