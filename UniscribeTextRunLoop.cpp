// UniscribeTextRunLoop.cpp

#include "UniscribeTextRunLoop.h"
#include <cassert>
#include <algorithm>

UniscribeTextRunLoop::UniscribeTextRunLoop( UTF16Ref text, const std::vector<SCRIPT_ITEM>& items, ArrayRef<const TextFontRun> fonts )
	: m_text( text )
	, m_position( 0 )
	, m_nextTab( 0 )
	, m_items( items )
	, m_item( m_items.begin() )
	, m_fonts( fonts )
	, m_font( m_fonts.begin() )
	, m_fontStart( 0 )
{
}

bool UniscribeTextRunLoop::Unfinished() const
{
	return m_item != m_items.end() - 1;
}

UniscribeTextRun UniscribeTextRunLoop::NextRun()
{
	size_t itemSize = m_item[1].iCharPos - m_position;
	size_t fontSize = m_font->count - ( m_position - m_fontStart );

	if ( m_nextTab <= m_position )
		m_nextTab = std::find( m_text.begin() + m_position, m_text.end(), wchar_t( '\t' ) ) - m_text.begin();

	UniscribeTextRun run( m_item - m_items.begin(), m_position, (std::min)( itemSize, (std::min)( fontSize, m_nextTab - m_position ) ), m_font->fontid );

	if ( run.textCount == 0 )
		run.textCount = 1;

	m_position += run.textCount;

	if ( m_position == m_item[1].iCharPos )
		++m_item;

	if ( m_position == m_fontStart + m_font->count )
	{
		m_fontStart += m_font->count;
		++m_font;
	}

	return run;
}

void UniscribeTextRunLoop::NewLine( size_t lineStart )
{
	m_position = lineStart;

	while ( size_t( m_item->iCharPos ) > m_position )
		--m_item;

	while ( m_fontStart > m_position )
	{
		--m_font;
		m_fontStart -= m_font->count;
	}
}
