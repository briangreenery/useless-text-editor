// TextEditRelevanceLexer.cpp

#include "TextEditRelevanceLexer.h"

TextEditRelevanceLexer::TextEditRelevanceLexer( const TextDocument& doc )
	: m_reader( doc )
	, m_offset( 0 )
{
}

void TextEditRelevanceLexer::Reset()
{
	RelevanceLexer::Reset();
	m_offset = 0;
}

void TextEditRelevanceLexer::Fill()
{
	const size_t minFillSize = 512;

	// If there's no more text to read, just return with what we have.
	if ( m_start && ( m_end - m_start < minFillSize ) )
		return;

	const UTF16::Unit* rangeStart = std::min( m_cursor, std::min( m_marker, m_ctxmarker ) );
	const UTF16::Unit* rangeEnd   = std::max( m_cursor, std::max( m_marker, m_ctxmarker ) );

	size_t markerOffset    = m_marker    - rangeStart;
	size_t ctxmarkerOffset = m_ctxmarker - rangeStart;
	size_t cursorOffset    = m_cursor    - rangeStart;

	if ( rangeEnd - rangeStart < minFillSize )
		rangeEnd = rangeStart + minFillSize;

	m_offset += rangeStart - m_start;
	UTF16Ref text = m_reader.WeakRange( m_offset, rangeEnd - rangeStart );

	m_start     = text.begin();
	m_end       = text.end();
	m_cursor    = m_start + cursorOffset;
	m_marker    = m_start + markerOffset;
	m_ctxmarker = m_start + ctxmarkerOffset;
}
