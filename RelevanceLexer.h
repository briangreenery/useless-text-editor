// RelevanceLexer.h

#ifndef RelevanceLexer_h
#define RelevanceLexer_h

#include "RelevanceToken.h"
#include "UString.h"

class RelevanceLexer
{
public:
	RelevanceLexer();

	RelevanceToken NextToken();

	virtual void Fill() = 0;

	const UTF16::Unit* start;
	const UTF16::Unit* end;
	const UTF16::Unit* cursor;
	const UTF16::Unit* marker;
	const UTF16::Unit* ctxmarker;
};

#endif
