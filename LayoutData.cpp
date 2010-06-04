// LayoutData.cpp

#include "LayoutData.h"

void LayoutData::Copy( LayoutAllocator& allocator )
{
	SizedAutoArray<SCRIPT_ITEM> itemsWithSentinel = allocator.items.Finish();
	size_t itemCount = itemsWithSentinel.size() - 1;
	items = SizedAutoArray<SCRIPT_ITEM>( itemsWithSentinel.Release(), itemCount );

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
