// UniscribeTextRunLoop.cpp

#include "UniscribeTextRunLoop.h"
#include "Assert.h"
#include <algorithm>

UniscribeTextRunLoop::UniscribeTextRunLoop( UTF16Ref text, const std::vector<SCRIPT_ITEM>& items )
	: m_text( text )
	, m_position( 0 )
	, m_items( &items.front(), &items.back() + 1 )
	, m_item( m_items.begin() )
	, m_itemUsed( 0 )
{
}

UniscribeTextRun UniscribeTextRunLoop::NextRun()
{
	Assert( Unfinished() );

	size_t itemSize = m_item[1].iCharPos - m_item[0].iCharPos;

	size_t runSize = 0;
	for ( ; runSize < itemSize - m_itemUsed; ++runSize )
		if ( m_text[m_position + runSize] == '\t' )
			break;

	if ( runSize == 0 )
		runSize = 1;

	UniscribeTextRun run;
	run.item      = m_item - m_items.begin();
	run.textStart = m_position;
	run.textCount = runSize;
	run.font      = 0;

	m_position += runSize;
	m_itemUsed += runSize;

	if ( m_itemUsed == itemSize )
	{
		++m_item;
		m_itemUsed = 0;
	}

	return run;
}

void UniscribeTextRunLoop::NewLine( size_t lineStart )
{
	m_position = lineStart;

	while ( size_t( m_item->iCharPos ) > m_position )
		--m_item;

	m_itemUsed = m_position - m_item->iCharPos;
}
