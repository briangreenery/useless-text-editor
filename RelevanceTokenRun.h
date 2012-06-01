// RelevanceToken.h

#ifndef RelevanceToken_h
#define RelevanceToken_h

#include "RelevanceLexerToken.h"
#include <vector>

class RelevanceTokenRun
{
public:
	RelevanceTokenRun( Relevance::LexerToken token, size_t start, size_t count );

	Relevance::LexerToken token;
	size_t start;
	size_t count;
};

typedef std::vector<RelevanceTokenRun> RelevanceTokenRuns;

#endif
