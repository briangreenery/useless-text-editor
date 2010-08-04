// TextRunLoop.h

#ifndef TextRunLoop_h
#define TextRunLoop_h

#include "LayoutAllocator.h"
#include "UString.h"
#include <windows.h>
#include <usp10.h>

class TextRunLoop
{
public:
	TextRunLoop( ArrayOf<SCRIPT_ITEM>,
	             UTF16Ref text,
	             LayoutAllocator& );

	TextRun* NextRun();
	bool Unfinished() const;

	void NewLine();
	void NewBlock();

	size_t BlockStart() const;
	UTF16Ref BlockText() const;
	ArrayOf<SCRIPT_ITEM> BlockItems() const;

private:
	VectorAllocator<TextRun>& m_allocator;

	ArrayOf<SCRIPT_ITEM> m_items;
	UTF16Ref             m_text;

	size_t m_position;
	size_t m_blockStart;

	SCRIPT_ITEM* m_item;
	size_t       m_itemUsed;
};

inline bool TextRunLoop::Unfinished() const
{
	return m_item != m_items.end();
}

inline size_t TextRunLoop::BlockStart() const
{
	return m_blockStart;
}

inline UTF16Ref TextRunLoop::BlockText() const
{
	return m_text;
}

inline ArrayOf<SCRIPT_ITEM> TextRunLoop::BlockItems() const
{
	return m_items;
}

#endif
