// UniscribeRunLoop.cpp

#include "UniscribeRunLoop.h"
#include <algorithm>

UniscribeRunLoop::UniscribeRunLoop( UTF16Ref text, UniscribeAllocator& allocator )
	: m_allocator( allocator.runs )
	, m_items( allocator.items.Allocated() )
	, m_text( text )
	, m_position( 0 )
	, m_item( allocator.items.Allocated().begin() )
	, m_itemUsed( 0 )
{
	m_allocator.Reset( m_items.size() );
}

UniscribeRun* UniscribeRunLoop::NextRun()
{
	Assert( Unfinished() );

	Assert( false ); // we have to scan for '\t'.  We can't assume it will be all alone at the beginning.

	size_t itemSize = m_item[1].iCharPos - m_item[0].iCharPos;
	size_t runSize  = ( m_text[m_position] == '\t' ) ? 1 : itemSize - m_itemUsed;

	UniscribeRun* run = m_allocator.Alloc( 1 ).begin();

	run->item      = m_item - m_items.begin();
	run->textStart = m_position;
	run->textCount = runSize;
	run->font      = 0;

	m_position += runSize;
	m_itemUsed += runSize;

	if ( m_itemUsed == itemSize )
	{
		++m_item;
		m_itemUsed = 0;
	}

	return run;
}

void UniscribeRunLoop::NewLine()
{
	Assert( !m_allocator.Allocated().empty() );

	UniscribeRun* lastRun = m_allocator.Allocated().end() - 1;
	m_position = lastRun->textStart + lastRun->textCount;

	while ( size_t( m_item->iCharPos ) > m_position )
		--m_item;

	m_itemUsed = m_position - m_item->iCharPos;
}
