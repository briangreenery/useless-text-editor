// SimpleTextRunLoop.h

#ifndef SimpleTextRunLoop_h
#define SimpleTextRunLoop_h

#include "SimpleTextRun.h"
#include "UString.h"

class SimpleTextRunLoop
{
public:
	SimpleTextRunLoop( UTF16Ref text );
	
	bool Unfinished() const;
	SimpleTextRun NextRun();
	void NewLine( size_t start );

private:
	size_t m_position;
	UTF16Ref m_text;
};

#endif
