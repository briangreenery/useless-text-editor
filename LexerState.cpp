// LexerState.cpp

#include "LexerState.h"
#include <algorithm>

#undef min
#undef max

LexerState::LexerState( const TextDocument& doc )
	: reader( doc )
	, offset( 0 )
	, start( 0 )
	, end( 0 )
	, marker( 0 )
	, ctxmarker( 0 )
{
}

const UTF16::Unit* LexerState::Fill( const UTF16::Unit* cursor )
{
	const UTF16::Unit* rangeStart = std::min( cursor, std::min( marker, ctxmarker ) );
	const UTF16::Unit* rangeEnd   = std::max( cursor, std::max( marker, ctxmarker ) );

	size_t markerOffset    = marker    - rangeStart;
	size_t ctxmarkerOffset = ctxmarker - rangeStart;
	size_t cursorOffset    = cursor    - rangeStart;

	if ( rangeEnd - rangeStart < 4096 )
		rangeEnd = rangeStart + 4096;

	offset += rangeStart - start;
	UTF16Ref text = reader.WeakRange( offset, rangeEnd - rangeStart );

	start     = text.begin();
	end       = text.end();
	marker    = start + markerOffset;
	ctxmarker = start + ctxmarkerOffset;

	return start + cursorOffset;
}
