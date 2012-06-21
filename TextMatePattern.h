// TextMatePattern.h

#ifndef TextMatePattern_h
#define TextMatePattern_h

#include <string>
#include <vector>
#include <stdint.h>
#include <oniguruma.h>

class TextStyleRegistry;

class TextMateCapture
{
public:
	TextMateCapture( uint32_t index, uint32_t classID );

	uint32_t index;
	uint32_t classID;
};

typedef std::vector<TextMateCapture> TextMateCaptures;

typedef std::shared_ptr<regex_t> OnigRegexPtr;
typedef std::shared_ptr<OnigRegion> OnigRegionPtr;

class TextMatePattern
{
public:
	TextMatePattern( uint32_t classID, OnigRegexPtr regex, const TextMateCaptures& captures );

	uint32_t classID;
	TextMateCaptures captures;
	OnigRegexPtr regex;

	OnigRegionPtr region;
	int matchStart;
	int matchLength;
};

typedef std::vector<TextMatePattern> TextMatePatterns;

TextMatePatterns ReadTextMateFile( const char* path, TextStyleRegistry& styleRegistry );

#endif
