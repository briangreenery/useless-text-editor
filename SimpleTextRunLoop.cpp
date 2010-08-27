// SimpleTextRunLoop.cpp

#include "SimpleTextRunLoop.h"
#include <algorithm>

SimpleTextRunLoop::SimpleTextRunLoop( UTF16Ref text )
	: m_text( text )
	, m_position( 0 )
{
}

bool SimpleTextRunLoop::Unfinished() const
{
	return m_position < m_text.size();
}

SimpleTextRun SimpleTextRunLoop::NextRun()
{
	SimpleTextRun run;

	const UTF16::Unit* nextTab = std::find( m_text.begin() + m_position, m_text.end(), UTF16::Unit( '\t' ) );

	run.font      = 0;
	run.textStart = m_position;
	run.textCount = nextTab - ( m_text.begin() + m_position );

	if ( run.textCount == 0 )
		run.textCount = 1;

	m_position += run.textCount;
	return run;
}

void SimpleTextRunLoop::NewLine( size_t start )
{
	m_position = start;
}
