// TextEditRelevanceLexer.h

#ifndef TextEditRelevanceLexer_h
#define TextEditRelevanceLexer_h

#include "RelevanceLexer.h"
#include "TextDocumentReader.h"

class TextEditRelevanceLexer : public RelevanceLexer
{
public:
	TextEditRelevanceLexer( const TextDocument& doc );

	void Reset();
	virtual void Fill();

private:
	size_t m_offset;
	TextDocumentReader m_reader;
};

#endif
