// NextRelevanceToken.h

#ifndef NextRelevanceToken_h
#define NextRelevanceToken_h

#include "UString.h"
#include "LexerState.h"

typedef unsigned char uint8;

namespace ColorType
{
	enum
	{
		automatic,
		keyword,
		op,
		number,
		string,
		ignored,
		illegal
	};
}

struct RelevanceToken
{
	uint8 colorType;
	uint8 lexType;
	uint8 parseType;
};

void NextRelevanceToken( RelevanceToken* token, LexerState* scanner, const UTF16::Unit* cursor );

#endif
