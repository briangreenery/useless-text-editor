// LexerState.h

#ifndef LexerState_h
#define LexerState_h

#include "DocumentReader.h"

class LexerState
{
public:
	LexerState( const TextDocument& doc );

	const UTF16::Unit* Fill( const UTF16::Unit* cursor );

	const UTF16::Unit* start;
	const UTF16::Unit* end;
	const UTF16::Unit* marker;
	const UTF16::Unit* ctxmarker;

private:
	size_t offset;
	DocumentReader reader;
};

#endif
