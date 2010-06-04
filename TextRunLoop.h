// TextRunLoop.h

#ifndef TextRunLoop_h
#define TextRunLoop_h

#include "LayoutAllocator.h"
#include "StyleRun.h"
#include "UString.h"
#include "Assert.h"
#include <windows.h>
#include <usp10.h>

class TextRunLoop
{
public:
	TextRunLoop( ArrayOf<SCRIPT_ITEM>,
				 ArrayOf<StyleRun>,
				 UTF16Ref text,
				 LayoutAllocator& );

	TextRun* NextRun();
	bool Unfinished() const;
	void NewLine();

private:
	VectorAllocator<TextRun>& m_allocator;

	ArrayOf<SCRIPT_ITEM> m_items;
	ArrayOf<StyleRun>    m_styles;
	UTF16Ref             m_text;

	size_t m_position;

	SCRIPT_ITEM* m_item;
	size_t       m_itemUsed;

	StyleRun* m_style;
	size_t    m_styleUsed;
};

inline bool TextRunLoop::Unfinished() const
{
	Assert( ( m_item == m_items.end() ) == ( m_style == m_styles.end() ) );
	return m_item != m_items.end();
}

#endif
