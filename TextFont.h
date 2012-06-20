// TextFont.h

#ifndef TextFont_h
#define TextFont_h

#include <Windows.h>
#include <usp10.h>
#include <string>
#include <memory>
#include <stdint.h>

class TextFont
{
public:
	TextFont( const std::wstring& name, int size, bool bold, bool italic );
	~TextFont();

	std::wstring name;
	int          size;
	bool         bold;
	bool         italic;

	HFONT                 hfont;
	mutable SCRIPT_CACHE  fontCache;
	SCRIPT_FONTPROPERTIES fontProps;
};

typedef std::shared_ptr<TextFont> TextFontPtr;

#endif
