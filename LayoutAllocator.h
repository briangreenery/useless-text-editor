// LayoutAllocator.h

#ifndef LayoutAllocator_h
#define LayoutAllocator_h

#include "VectorAllocator.h"
#include "VectorCache.h"
#include "TextRun.h"
#include <windows.h>
#include <usp10.h>

class LayoutAllocator
{
public:
	void Reset( size_t paragraphLength );
	void DiscardFrom( size_t textStart, size_t glyphStart );

	static size_t EstimateGlyphCount( size_t textSize ) { return MulDiv( 3, textSize, 2 ) + 16; }

	// Paragraph data
	VectorAllocator<TextRun> runs;
	VectorAllocator<size_t>  lines;

	// ScriptItemize data
	VectorAllocator<SCRIPT_ITEM> items;

	// ScriptShape data
	VectorAllocator<WORD>           logClusters;
	VectorAllocator<WORD>           glyphs;
	VectorAllocator<SCRIPT_VISATTR> visAttrs;

	// ScriptPlace data
	VectorAllocator<int>     advanceWidths;
	VectorAllocator<GOFFSET> offsets;

	// ScriptBreak data
	VectorCache<SCRIPT_LOGATTR> logAttr;

	// ScriptGetLogicalWidths data
	VectorCache<int> logWidths;
};

#endif
