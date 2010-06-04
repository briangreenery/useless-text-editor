// LayoutEngine.h

#ifndef LayoutEngine_h
#define LayoutEngine_h

#include "LayoutAllocator.h"
#include "ArrayOf.h"
#include "UString.h"
#include "TextRun.h"
#include "StyleRun.h"
#include <limits>
#include <Windows.h>

class LayoutEngine
{
public:
	LayoutEngine( HFONT font, SCRIPT_CACHE fontCache, HDC hdc, int maxWidth, int tabSize );

	void LayoutParagraph( UTF16Ref text, ArrayOf<StyleRun> style );

	LayoutAllocator& Result() { return m_allocator; }

private:
	ArrayOf<SCRIPT_ITEM> Itemize( UTF16Ref );
	int ShapePlaceRun( const UTF16::Unit*, ArrayOf<SCRIPT_ITEM>, TextRun*, int xStart );

	size_t EstimateLineWrap( const UTF16::Unit*, ArrayOf<SCRIPT_ITEM>, TextRun*, int lineWidth );
	size_t ForceLineWrap( const UTF16::Unit*, ArrayOf<SCRIPT_ITEM>, TextRun* );
	size_t FindSoftBreak( const UTF16::Unit*, SCRIPT_ITEM*, size_t estimate );
	TextRun* DiscardRunsAfter( size_t lineEnd );
	size_t WrapLine( const UTF16::Unit*, ArrayOf<SCRIPT_ITEM>, TextRun*, size_t lineStart, int lineWidth );

	void AddLine();

	HDC m_hdc;
	int m_maxWidth;
	int m_tabSize;

	HFONT        m_font;
	SCRIPT_CACHE m_fontCache;

	LayoutAllocator m_allocator;

	static const size_t noSoftBreak = 0xffffffff; /*std::numeric_limits<size_t>::max();*/
};

#endif
