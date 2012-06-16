// SimpleTextRunLoop.cpp

#include "SimpleTextRunLoop.h"
#include <algorithm>

SimpleTextRunLoop::SimpleTextRunLoop( UTF16Ref text, ArrayRef<const TextFontRun> fonts )
	: m_position( 0 )
	, m_nextTab( 0 )
	, m_text( text )
	, m_fonts( fonts )
	, m_font( m_fonts.begin() )
	, m_fontStart( 0 )
{
}

bool SimpleTextRunLoop::Unfinished() const
{
	return m_position < m_text.size();
}

SimpleTextRun SimpleTextRunLoop::NextRun()
{
	size_t fontRunSize = m_font->count - ( m_position - m_fontStart );

	if ( m_nextTab <= m_position )
		m_nextTab = std::find( m_text.begin() + m_position, m_text.end(), wchar_t( '\t' ) ) - m_text.begin();

	SimpleTextRun run( m_position, std::min( fontRunSize, m_nextTab - m_position ), m_font->fontid );

	if ( run.textCount == 0 )
		run.textCount = 1;

	m_position += run.textCount;

	if ( m_position == m_fontStart + m_font->count )
	{
		m_fontStart += m_font->count;
		++m_font;
	}

	return run;
}

void SimpleTextRunLoop::NewLine( size_t start )
{
	m_position = start;

	while ( m_fontStart > m_position )
	{
		--m_font;
		m_fontStart -= m_font->count;
	}
}
