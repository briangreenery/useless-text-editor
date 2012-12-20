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
	TextMateRegex( OnigRegexPtr regex, const std::vector<TextMateCapture>& captures );

	OnigRegexPtr regex;
	std::vector<TextMateCapture> captures;

	OnigRegionPtr region;
	int32_t matchStart;
	int32_t matchLength;
};

typedef std::shared_ptr<TextMateRegex> TextMateRegexPtr;

class TextMatePatterns
{
public:
	std::vector<TextMateRegex*> begins;
	std::vector<TextMateRegex*> ends;
	std::vector<TextMatePatterns*> patterns;
	std::vector<uint32_t> styles;
};

typedef std::shared_ptr<TextMatePatterns> TextMatePatternsPtr;

class TextMateLanguage
{
public:
	TextMateLanguage();

	TextMateRegex* NewRegex( const char* pattern, const std::vector<TextMateCapture>& );
	TextMatePatterns* NewPatterns();

	std::vector<TextMateRegexPtr> regexes;
	std::vector<TextMatePatternsPtr> patterns;
	TextMatePatterns* defaultPatterns;
};

class TextMateStack
{
public:
	bool IsEmpty() const;
	void Push( uint32_t style, TextMateRegex* end, TextMatePatterns* patterns );
	void PopTo( int32_t index );

	std::vector<TextMatePatterns*> patternsList;
	std::vector<TextMateRegex*> ends;
	std::vector<uint32_t> styles;
};

TextMateLanguage ReadTextMateFile( const char* path, TextStyleRegistry& styleRegistry );

// Match begin, apply patterns
// If there is an end, maybe re-generate the end regex using the matched patterns (not yet)
// Lookup the patterns

// maybe just use vectors of integers for everything, might be simplest.

#endif
