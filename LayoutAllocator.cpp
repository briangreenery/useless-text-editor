// LayoutAllocator.cpp

#include "LayoutAllocator.h"

void LayoutAllocator::Reset( size_t paragraphLength )
{
	size_t estimatedGlyphCount = EstimateGlyphCount( paragraphLength );

	lines        .Reset( 1 );
	items        .Reset( paragraphLength + 1 );
	logClusters  .Reset( paragraphLength     );
	glyphs       .Reset( estimatedGlyphCount );
	visAttrs     .Reset( estimatedGlyphCount );
	advanceWidths.Reset( estimatedGlyphCount );
	offsets      .Reset( estimatedGlyphCount );
}

void LayoutAllocator::DiscardFrom( size_t textStart, size_t glyphStart )
{
	logClusters  .DiscardFrom( textStart  );
	glyphs       .DiscardFrom( glyphStart );
	visAttrs     .DiscardFrom( glyphStart );
	advanceWidths.DiscardFrom( glyphStart );
	offsets      .DiscardFrom( glyphStart );
}
