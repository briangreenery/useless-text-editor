// UniscribeTextRunLoop.h

#ifndef UniscribeTextRunLoop_h
#define UniscribeTextRunLoop_h

#include "UniscribeLayoutData.h"
#include "ArrayOf.h"
#include "UString.h"
#include <windows.h>
#include <usp10.h>

class UniscribeTextRunLoop
{
public:
	UniscribeTextRunLoop( UTF16Ref, const std::vector<SCRIPT_ITEM>& );

	bool Unfinished() const;
	UniscribeTextRun NextRun();
	void NewLine( size_t lineStart );

private:
	UTF16Ref m_text;
	size_t   m_position;

	ArrayOf<const SCRIPT_ITEM> m_items;
	const SCRIPT_ITEM*         m_item;
	size_t                     m_itemUsed;
};

inline bool UniscribeTextRunLoop::Unfinished() const
{
	return m_item != m_items.end() - 1;
}

#endif
