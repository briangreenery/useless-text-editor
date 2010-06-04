// TextRunLoop.cpp

#include "TextRunLoop.h"
#include <algorithm>

#undef min
#undef max

TextRunLoop::TextRunLoop( ArrayOf<SCRIPT_ITEM> items,
                          ArrayOf<StyleRun> styles,
                          UTF16Ref text,
                          LayoutAllocator& allocator )
	: m_allocator( allocator.runs )
	, m_items( items )
	, m_styles( styles )
	, m_text( text )
	, m_position( 0 )
	, m_item( items.begin() )
	, m_itemUsed( 0 )
	, m_style( styles.begin() )
	, m_styleUsed( 0 )
{
	m_allocator.Reset( items.size() );
}

TextRun* TextRunLoop::NextRun()
{
	Assert( Unfinished() );

	size_t itemSize  = m_item [1].iCharPos - m_item [0].iCharPos;
	size_t styleSize = m_style[1].start    - m_style[0].start;

	bool isTab = ( m_text.begin()[m_position] == '\t' );

	size_t runSize = isTab ? 1 : std::min( itemSize - m_itemUsed, styleSize - m_styleUsed );

	TextRun* run   = m_allocator.Alloc( 1 ).begin();
	run->item      = m_item - m_items.begin();
	run->style     = m_style->style;
	run->textStart = m_position;
	run->textCount = runSize;

	m_position  += runSize;
	m_itemUsed  += runSize;
	m_styleUsed += runSize;

	if ( m_itemUsed == itemSize )
	{
		++m_item;
		m_itemUsed = 0;
	}

	if ( m_styleUsed == styleSize )
	{
		++m_style;
		m_styleUsed = 0;
	}

	return run;
}

void TextRunLoop::NewLine()
{
	Assert( !m_allocator.Allocated().empty() );

	TextRun* lastRun = m_allocator.Allocated().end() - 1;
	m_position = lastRun->textStart + lastRun->textCount;

	while ( size_t( m_item->iCharPos ) > m_position )
		--m_item;

	while ( m_style->start > m_position )
		--m_style;

	m_itemUsed  = m_position - m_item->iCharPos;
	m_styleUsed = m_position - m_style->start;
}
