// TextStyle.h

#ifndef TextStyle_h
#define TextStyle_h

#include "TextFont.h"
#include <vector>

class TextStyle
{
public:
	TextStyle();
	~TextStyle();

	size_t AddFont( LPCWSTR name );
	void SetDefaultFont( size_t font );

	int fontSize;
	int lineHeight;
	int avgCharWidth;
	int tabSize;

	HBRUSH gutterBrush;
	HBRUSH selectionBrush;

	size_t defaultFont;
	std::vector<TextFont> fonts;

private:
	void AddJapaneseFont();
	void AddChineseFont();
	void AddKoreanFont();
	void AddFallbackFonts();
};

#endif
