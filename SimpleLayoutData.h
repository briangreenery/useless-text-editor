// SimpleLayoutData.h

#ifndef SimpleLayoutData_h
#define SimpleLayoutData_h

#include "SimpleTextRun.h"
#include "UString.h"
#include <Windows.h>
#include <vector>
#include <memory>

class SimpleLayoutData
{
public:
	SimpleLayoutData( UTF16Ref text, bool endsWithNewline );

	void DiscardFrom( size_t position );

	size_t                     length;
	bool                       endsWithNewline;
	std::vector<size_t>        lines;
	std::vector<SimpleTextRun> runs;
	std::vector<INT>           xOffsets;
};

typedef std::unique_ptr<SimpleLayoutData> SimpleLayoutDataPtr;

#endif
