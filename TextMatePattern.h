// TextMatePattern.h

#ifndef TextMatePattern_h
#define TextMatePattern_h

#include <vector>
#include <stdint.h>
#include <memory>
#include <oniguruma.h>

class TextStyleRegistry;

class TextMateCapture
{
public:
	TextMateCapture( uint32_t index, uint32_t style );

	uint32_t index;
	uint32_t style;
};

typedef std::vector<TextMateCapture> TextMateCaptures;

typedef std::shared_ptr<regex_t> OnigRegexPtr;
typedef std::shared_ptr<OnigRegion> OnigRegionPtr;

class TextMateBestMatch
{
public:
	TextMateBestMatch();

	bool IsBetterThan( int32_t otherStart, int32_t otherLength ) const;

	int32_t index;
	int32_t start;
	int32_t length;
};

class TextMateRegex
{
public:
	TextMateRegex( OnigRegexPtr regex );

	OnigRegexPtr regex;
	OnigRegionPtr region;
	int32_t matchStart;
};

typedef std::shared_ptr<TextMateRegex> TextMateRegexPtr;

class TextMatePatterns
{
public:
	std::vector<TextMateRegex*> regexes;
	std::vector<TextMateCaptures> captures;
	std::vector<TextMatePatterns*> patterns;
	std::vector<uint32_t> styles;
};

typedef std::shared_ptr<TextMatePatterns> TextMatePatternsPtr;

class TextMateLanguage
{
public:
	TextMateLanguage();

	TextMateRegex* NewRegex( const char* pattern );
	TextMatePatterns* NewPatterns();

	std::vector<TextMateRegexPtr> regexes;
	std::vector<TextMatePatternsPtr> patterns;
	TextMatePatterns* defaultPatterns;
};

TextMateLanguage ReadTextMateFile( const char* path, TextStyleRegistry& styleRegistry );

#endif
