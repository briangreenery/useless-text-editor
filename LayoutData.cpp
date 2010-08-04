// LayoutData.cpp

#include "LayoutData.h"

void LayoutData::Copy( LayoutAllocator& allocator, ArrayOf<SCRIPT_ITEM> itemRange )
{
	items = CreateArray<SCRIPT_ITEM>( itemRange.size() );
	std::copy( itemRange.begin(), itemRange.end(), items.begin() );

	runs          = allocator.runs         .Finish();
	logClusters   = allocator.logClusters  .Finish();
	glyphs        = allocator.glyphs       .Finish();
	visAttrs      = allocator.visAttrs     .Finish();
	advanceWidths = allocator.advanceWidths.Finish();
	offsets       = allocator.offsets      .Finish();
}

void LayoutData::Reset()
{
	items        .Reset();
	runs         .Reset();
	logClusters  .Reset();
	glyphs       .Reset();
	visAttrs     .Reset();
	advanceWidths.Reset();
	offsets      .Reset();
}
