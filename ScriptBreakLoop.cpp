// ScriptBreakLoop.cpp

#include "ScriptBreakLoop.h"
#include "DocumentReader.h"
#include "Error.h"
#include <algorithm>

namespace W = Windows;

ScriptBreakLoop::ScriptBreakLoop( size_t pos, const DocumentReader& reader, ArrayOf<SCRIPT_ITEM> items )
	: m_pos( pos )
	, m_reader( reader )
	, m_items( items )
{
	m_item = ItemContaining( pos );

	if ( Unfinished() )
		BreakItem();
}

void ScriptBreakLoop::operator++()
{
	++m_attr;
	++m_pos;

	if ( m_attr == m_attrs.end() )
	{
		++m_item;

		if ( Unfinished() )
			BreakItem();
	}
}

void ScriptBreakLoop::operator--()
{
	--m_attr;

	if ( m_pos > 0 )
		--m_pos;

	if ( m_attr == m_attrs.rend() )
	{
		--m_item;

		if ( Unfinished() )
			BreakItem();
	}
}

void ScriptBreakLoop::BreakItem()
{
	size_t textStart = m_item[0].iCharPos;
	size_t textSize  = m_item[1].iCharPos - m_item[0].iCharPos;

	UTF16Ref text = m_reader.StrictRange( textStart, textSize );
	m_attrs = m_cache.TempArray( text.size() );

	W::ThrowHRESULT( ScriptBreak( text.begin(), text.size(), &m_item->a, m_attrs.begin() ) );

	m_attr = m_attrs.begin() + ( m_pos - textStart );
}

struct CompareItems
{
	bool operator()( const SCRIPT_ITEM& item, size_t pos ) const
	{
		return size_t( (&item)[1].iCharPos ) <= pos;
	}

	bool operator()( size_t pos, const SCRIPT_ITEM& item ) const
	{
		return pos < size_t( item.iCharPos );
	}

	bool operator()( const SCRIPT_ITEM& a, const SCRIPT_ITEM& b ) const
	{
		return a.iCharPos < b.iCharPos;
	}
};

const SCRIPT_ITEM* ScriptBreakLoop::ItemContaining( size_t pos ) const
{
	return std::lower_bound( m_items.begin(), m_items.end(), pos, CompareItems() );
}
