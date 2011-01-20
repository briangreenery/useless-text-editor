//
//// LexerState.cpp
//
//#include "LexerState.h"
//#include "NextRelevanceToken.h"
//#include <algorithm>
//
//#undef min
//#undef max
//
//LexerState::LexerState( const TextDocument& doc )
//	: reader( doc )
//	, offset( 0 )
//	, start( 0 )
//	, end( 0 )
//	, cursor( 0 )
//	, marker( 0 )
//	, ctxmarker( 0 )
//{
//}
//
//void LexerState::Fill()
//{
//	const UTF16::Unit* rangeStart = std::min( cursor, std::min( marker, ctxmarker ) );
//	const UTF16::Unit* rangeEnd   = std::max( cursor, std::max( marker, ctxmarker ) );
//
//	size_t markerOffset    = marker    - rangeStart;
//	size_t ctxmarkerOffset = ctxmarker - rangeStart;
//	size_t cursorOffset    = cursor    - rangeStart;
//
//	if ( rangeEnd - rangeStart < 512 )
//		rangeEnd = rangeStart + 512;
//
//	offset += rangeStart - start;
//	UTF16Ref text = reader.WeakRange( offset, rangeEnd - rangeStart );
//
//	start     = text.begin();
//	end       = text.end();
//	cursor    = start + cursorOffset;
//	marker    = start + markerOffset;
//	ctxmarker = start + ctxmarkerOffset;
//}
//
//void LexDocument( const TextDocument& doc )
//{
//	LexerState state( doc );
//	std::vector<RelevanceToken> tokens;
//
//	while ( true )
//	{
//		RelevanceToken token = NextRelevanceToken( state );
//
//		if ( token == RelevanceToken::t_endOfInput )
//			break;
//
//		tokens.push_back( token );
//	}
//}
