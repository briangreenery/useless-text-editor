// LayoutData.h

#ifndef LayoutData_h
#define LayoutData_h

#include "LayoutAllocator.h"

class LayoutData
{
public:
	void Copy( LayoutAllocator& );
	void Reset();

	SizedAutoArray<SCRIPT_ITEM>    items;
	SizedAutoArray<TextRun>        runs;

	SizedAutoArray<WORD>           logClusters;
	SizedAutoArray<WORD>           glyphs;
	SizedAutoArray<SCRIPT_VISATTR> visAttrs;
	SizedAutoArray<int>            advanceWidths;
	SizedAutoArray<GOFFSET>        offsets;
};

#endif
