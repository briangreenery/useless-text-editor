// UniscribeLayoutData.h

#ifndef UniscribeLayoutData_h
#define UniscribeLayoutData_h

#include "UniscribeTextRun.h"
#include "UString.h"
#include <memory>
#include <vector>
#include <windows.h>
#include <usp10.h>

class UniscribeLayoutData
{
public:
	UniscribeLayoutData( UTF16Ref text, bool endsWithNewline );

	void AddRun( UniscribeTextRun,
	             const std::vector<WORD>& logClusters,
	             const std::vector<WORD>& glyphs,
	             const std::vector<SCRIPT_VISATTR>& visAttrs,
	             const std::vector<int>& advanceWidths,
	             const std::vector<GOFFSET>& offsets );

	UniscribeTextRun DiscardFrom( size_t position );

	size_t length;
	bool   endsWithNewline;

	// Block data
	std::vector<UniscribeTextRun> runs;
	std::vector<size_t>           lines;

	// ScriptItemize data
	std::vector<SCRIPT_ITEM> items;

	// ScriptShape data
	std::vector<WORD>           logClusters;
	std::vector<WORD>           glyphs;
	std::vector<SCRIPT_VISATTR> visAttrs;

	// ScriptPlace data
	std::vector<int>     advanceWidths;
	std::vector<GOFFSET> offsets;
};

typedef std::unique_ptr<UniscribeLayoutData> UniscribeLayoutDataPtr;

#endif
