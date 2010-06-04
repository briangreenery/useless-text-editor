// ScriptBreakLoop.h

#ifndef ScriptBreakLoop_h
#define ScriptBreakLoop_h

#include "ArrayOf.h"
#include "VectorCache.h"
#include <Windows.h>
#include <usp10.h>

class DocumentReader;

class ScriptBreakLoop
{
public:
	ScriptBreakLoop( size_t pos, const DocumentReader&, ArrayOf<SCRIPT_ITEM> items );

	size_t Pos() const { return m_pos; }

	bool Unfinished() const;
	void operator++();
	void operator--();

	const SCRIPT_LOGATTR* operator* () const { return m_attr; }
	const SCRIPT_LOGATTR* operator->() const { return m_attr; }

private:
	void BreakItem();
	const SCRIPT_ITEM* ItemContaining( size_t pos ) const;

	size_t m_pos;
	const DocumentReader& m_reader;

	const SCRIPT_ITEM* m_item;
	ArrayOf<SCRIPT_ITEM> m_items;

	const SCRIPT_LOGATTR* m_attr;
	ArrayOf<SCRIPT_LOGATTR> m_attrs;

	VectorCache<SCRIPT_LOGATTR> m_cache;
};

inline bool ScriptBreakLoop::Unfinished() const
{
	return m_item != m_items.end()
		&& m_item != m_items.rend();
}

#endif
