// TextEditRelevanceLexer.h

#ifndef TextEditRelevanceLexer_h
#define TextEditRelevanceLexer_h

#include "RelevanceLexer.h"
#include "TextDocumentReader.h"

class TextEditRelevanceLexer : public RelevanceLexer
{
public:
	TextEditRelevanceLexer( const TextDocument& doc );

	size_t Position() const { return m_offset + ( m_cursor - m_start ); }

	void Reset();
	virtual void Fill();

private:
	size_t m_offset;
	TextDocumentReader m_reader;
};

#endif
