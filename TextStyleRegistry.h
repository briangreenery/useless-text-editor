// TextStyleRegistry.h

#ifndef TextStyleRegistry_h
#define TextStyleRegistry_h

#include "TextFont.h"
#include "TextStyle.h"
#include <map>

class TextAnnotator;

class TextStyleRegistry
{
public:
	TextStyleRegistry();

	uint32_t AddFont( LPCWSTR name );
	void RemoveFont( uint32_t );
	void SetDefaultFont( uint32_t );

	uint32_t AddStyle( const TextStyle& );
	void RemoveStyle( uint32_t );
	void SetDefaultStyle( uint32_t );

	const TextStyle& Style( uint32_t styleid ) const;
	const TextFont& Font( uint32_t fontid ) const;

	int fontSize;
	int lineHeight;
	int avgCharWidth;
	int tabSize;

	COLORREF gutterColor;
	COLORREF selectionColor;
	COLORREF defaultBkColor;
	COLORREF defaultTextColor;

	const uint32_t defaultFontid;
	const uint32_t defaultStyleid;

	typedef std::map<uint32_t,TextStyle> StyleMap;
	typedef std::map<uint32_t,TextFontPtr> FontMap;

	StyleMap styles;
	FontMap fonts;

	TextAnnotator* annotator;

private:
	TextFontPtr CreateFont( LPCWSTR name );
	void AddFallbackFonts();

	TextFontPtr defaultFont;
	TextStyle defaultStyle;

	uint32_t nextStyle;
	uint32_t nextFont;
};

#endif
