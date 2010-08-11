// UniscribeAllocator.cpp

#include "UniscribeAllocator.h"

void UniscribeAllocator::DiscardFrom( size_t textStart, size_t glyphStart )
{
	logClusters  .DiscardFrom( textStart  );
	glyphs       .DiscardFrom( glyphStart );
	visAttrs     .DiscardFrom( glyphStart );
	advanceWidths.DiscardFrom( glyphStart );
	offsets      .DiscardFrom( glyphStart );
}

void UniscribeAllocator::DiscardAll()
{
	runs         .DiscardAll();
	lines        .DiscardAll();
	items        .DiscardAll();
	logClusters  .DiscardAll();
	glyphs       .DiscardAll();
	visAttrs     .DiscardAll();
	advanceWidths.DiscardAll();
	offsets      .DiscardAll();
}