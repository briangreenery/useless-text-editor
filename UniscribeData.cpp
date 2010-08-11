// UniscribeData.cpp

#include "UniscribeData.h"

UniscribeData::UniscribeData( UniscribeAllocator& allocator )
{
	items         = allocator.items        .Finish();
	runs          = allocator.runs         .Finish();
	logClusters   = allocator.logClusters  .Finish();
	glyphs        = allocator.glyphs       .Finish();
	visAttrs      = allocator.visAttrs     .Finish();
	advanceWidths = allocator.advanceWidths.Finish();
	offsets       = allocator.offsets      .Finish();
}
