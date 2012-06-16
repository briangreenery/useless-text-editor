// UniscribeLayoutData.cpp

#include "UniscribeLayoutData.h"
#include "Assert.h"

UniscribeLayoutData::UniscribeLayoutData( UTF16Ref text, bool endsWithNewline )
	: length( text.size() )
	, endsWithNewline( endsWithNewline )
{
	if ( endsWithNewline )
		length++;
}

void UniscribeLayoutData::AddRun( UniscribeTextRun run,
                                  const std::vector<WORD>& runLogClusters,
                                  const std::vector<WORD>& runGlyphs,
                                  const std::vector<SCRIPT_VISATTR>& runVisAttrs,
                                  const std::vector<int>& runAdvanceWidths,
                                  const std::vector<GOFFSET>& runOffsets )
{
	runs.push_back( run );

	logClusters  .insert( logClusters  .end(), runLogClusters  .begin(), runLogClusters  .end() );
	glyphs       .insert( glyphs       .end(), runGlyphs       .begin(), runGlyphs       .end() );
	visAttrs     .insert( visAttrs     .end(), runVisAttrs     .begin(), runVisAttrs     .end() );
	advanceWidths.insert( advanceWidths.end(), runAdvanceWidths.begin(), runAdvanceWidths.end() );
	offsets      .insert( offsets      .end(), runOffsets      .begin(), runOffsets      .end() );
}

UniscribeTextRun UniscribeLayoutData::DiscardFrom( size_t position )
{
	Assert( !runs.empty() );
	Assert( position <= runs.back().textStart + runs.back().textCount );
	Assert( runs.front().textStart == 0 );

	size_t i = runs.size() - 1;
	while ( runs[i].textStart > position )
		--i;

	UniscribeTextRun run = runs[i];
	run.textCount = position - run.textStart;

	runs         .erase( runs         .begin() + i,              runs         .end() );
	logClusters  .erase( logClusters  .begin() + run.textStart,  logClusters  .end() );
	glyphs       .erase( glyphs       .begin() + run.glyphStart, glyphs       .end() );
	visAttrs     .erase( visAttrs     .begin() + run.glyphStart, visAttrs     .end() );
	advanceWidths.erase( advanceWidths.begin() + run.glyphStart, advanceWidths.end() );
	offsets      .erase( offsets      .begin() + run.glyphStart, offsets      .end() );

	return run;
}
