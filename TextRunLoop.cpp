// TextRunLoop.cpp

#include "TextRunLoop.h"
#include <algorithm>

TextRunLoop::TextRunLoop( ArrayOf<SCRIPT_ITEM> items,
                          UTF16Ref text,
                          LayoutAllocator& allocator )
	: m_allocator( allocator.runs )
	, m_items( items )
	, m_text( text )
	, m_position( 0 )
	, m_blockStart( 0 )
	, m_item( items.begin() )
	, m_itemUsed( 0 )
{
	m_allocator.Reset( items.size() );
}

TextRun* TextRunLoop::NextRun()
{
	Assert( Unfinished() );

	size_t itemSize = m_item[1].iCharPos - m_item[0].iCharPos;
	size_t runSize  = ( m_text[m_position] == '\t' ) ? 1 : itemSize - m_itemUsed;

	TextRun* run   = m_allocator.Alloc( 1 ).begin();
	run->item      = m_item - m_items.begin();
	run->textStart = m_position;
	run->textCount = runSize;
	run->style     = 0;

	m_position += runSize;
	m_itemUsed += runSize;

	if ( m_itemUsed == itemSize )
	{
		++m_item;
		m_itemUsed = 0;
	}

	return run;
}

void TextRunLoop::NewLine()
{
	Assert( !m_allocator.Allocated().empty() );

	TextRun* lastRun = m_allocator.Allocated().end() - 1;
	m_position = lastRun->textStart + lastRun->textCount;

	while ( size_t( m_item->iCharPos ) > m_position + m_blockStart )
		--m_item;

	m_itemUsed = ( m_position + m_blockStart ) - m_item->iCharPos;
}

void TextRunLoop::NewBlock()
{
	NewLine();

	m_text = UTF16Ref( m_text.begin() + m_position, m_text.end() );
	m_items = ArrayOf<SCRIPT_ITEM>( m_item, m_items.end() );

	m_blockStart += m_position;
	m_position = 0;

	m_item->iCharPos = m_blockStart;
	m_itemUsed = 0;
}
