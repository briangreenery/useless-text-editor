// TextTheme.h

#ifndef TextTheme_h
#define TextTheme_h

#include "TextFont.h"
#include "TextStyle.h"
#include <string>
#include <map>

class TextStyleRegistry;

class TextTheme
{
public:
	TextTheme();

	uint32_t AddFont( const std::wstring& name, bool bold, bool italic );
	const TextFont& Font( uint32_t fontID ) const;

	void SetStyle( const std::string& className, const TextStyle& style );
	const TextStyle& Style( std::string className ) const;

	int fontSize;
	int tabWidth;

	COLORREF selectionColor;
	COLORREF gutterBkColor;
	COLORREF gutterTextColor;
	COLORREF gutterLineColor;

private:
	uint32_t m_nextFontID;

	typedef std::map<uint32_t,TextFontPtr> FontMap;
	FontMap m_fonts;

	typedef std::map<std::string,TextStyle> StyleMap;
	StyleMap m_styles;
};

#endif
