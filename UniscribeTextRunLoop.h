// UniscribeTextRunLoop.h

#ifndef UniscribeTextRunLoop_h
#define UniscribeTextRunLoop_h

#include "UniscribeLayoutData.h"
#include "ArrayRef.h"
#include "UTF16Ref.h"
#include "TextFontRun.h"
#include <windows.h>
#include <usp10.h>

class UniscribeTextRunLoop
{
public:
	UniscribeTextRunLoop( UTF16Ref, const std::vector<SCRIPT_ITEM>&, ArrayRef<const TextFontRun> fonts );

	bool Unfinished() const;
	UniscribeTextRun NextRun();
	void NewLine( size_t lineStart );

private:
	UTF16Ref m_text;

	size_t m_position;
	size_t m_nextTab;

	const std::vector<SCRIPT_ITEM>           m_items;
	std::vector<SCRIPT_ITEM>::const_iterator m_item;

	ArrayRef<const TextFontRun> m_fonts;
	const TextFontRun*         m_font;
	size_t                     m_fontStart;
};

#endif
