// UniscribeRunLoop.h

#ifndef UniscribeRunLoop_h
#define UniscribeRunLoop_h

#include "UniscribeAllocator.h"
#include "UString.h"
#include <windows.h>
#include <usp10.h>

class UniscribeRunLoop
{
public:
	UniscribeRunLoop( UTF16Ref, UniscribeAllocator& );

	bool Unfinished() const;
	UniscribeRun* NextRun();
	void NewLine();

private:
	VectorAllocator<UniscribeRun>& m_allocator;

	ArrayOf<SCRIPT_ITEM> m_items;
	UTF16Ref             m_text;

	size_t m_position;

	SCRIPT_ITEM* m_item;
	size_t       m_itemUsed;
};

inline bool UniscribeRunLoop::Unfinished() const
{
	return m_item != m_items.end() - 1;
}

#endif
