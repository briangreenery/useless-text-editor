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

	uint32 AddFont( LPCWSTR name );
	void RemoveFont( uint32 );
	void SetDefaultFont( uint32 );

	uint32 AddStyle( const TextStyle& );
	void RemoveStyle( uint32 );
	void SetDefaultStyle( uint32 );

	const TextStyle& Style( uint32 styleid ) const;
	const TextFont& Font( uint32 fontid ) const;

	int fontSize;
	int lineHeight;
	int avgCharWidth;
	int tabSize;

	COLORREF gutterColor;
	COLORREF selectionColor;
	COLORREF defaultBkColor;
	COLORREF defaultTextColor;

	const uint32 defaultFontid;
	const uint32 defaultStyleid;

	typedef std::map<uint32,TextStyle> StyleMap;
	typedef std::map<uint32,TextFontPtr> FontMap;

	StyleMap styles;
	FontMap fonts;

	TextAnnotator* annotator;

private:
	TextFontPtr CreateFont( LPCWSTR name );
	void AddFallbackFonts();

	TextFontPtr defaultFont;
	TextStyle defaultStyle;

	uint32 nextStyle;
	uint32 nextFont;
};

#endif
