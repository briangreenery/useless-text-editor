// RelevanceLexer.h

#ifndef RelevanceLexer_h
#define RelevanceLexer_h

#include "RelevanceToken.h"
#include "UString.h"

class RelevanceLexer
{
public:
	RelevanceLexer();

	void Reset();
	RelevanceToken NextToken();

	virtual void Fill() = 0;

protected:
	const UTF16::Unit* m_start;
	const UTF16::Unit* m_end;
	const UTF16::Unit* m_cursor;
	const UTF16::Unit* m_marker;
	const UTF16::Unit* m_ctxmarker;
};

#endif
