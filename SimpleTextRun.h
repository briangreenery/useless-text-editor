// SimpleTextRun.h

#ifndef SimpleTextRun_h
#define SimpleTextRun_h

#include <stdint.h>

class SimpleTextRun
{
public:
	SimpleTextRun() {}
	SimpleTextRun( size_t textStart, size_t textCount, size_t font );
	
	size_t textStart;
	size_t textCount;
	uint32_t fontid;
};

#endif
