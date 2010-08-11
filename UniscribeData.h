// UniscibeData.h

#ifndef UniscibeData_h
#define UniscibeData_h

#include "UniscribeAllocator.h"
#include <memory>

class UniscribeData
{
public:
	UniscribeData( UniscribeAllocator& );

	SizedAutoArray<SCRIPT_ITEM>    items;
	SizedAutoArray<UniscribeRun>   runs;
	SizedAutoArray<WORD>           logClusters;
	SizedAutoArray<WORD>           glyphs;
	SizedAutoArray<SCRIPT_VISATTR> visAttrs;
	SizedAutoArray<int>            advanceWidths;
	SizedAutoArray<GOFFSET>        offsets;
};

typedef std::shared_ptr<UniscribeData> UniscribeDataPtr;

#endif
