// TextStyleRegistry.h

#ifndef TextStyleRegistry_h
#define TextStyleRegistry_h

#include "TextFont.h"
#include "TextStyle.h"
#include "TextTheme.h"
#include <map>
#include <set>

class TextAnnotator;

class TextStyleRegistry
{
public:
	TextStyleRegistry();

	uint32_t ClassID( const std::string& className );

	void SetAnnotator( TextAnnotator* );
	TextAnnotator* Annotator() const;

	void SetTheme( const TextTheme& );
	const TextTheme& Theme() const;

	const TextStyle& Style( uint32_t classID ) const;
	const TextFont&  Font ( uint32_t fontID  ) const;

	const TextStyle& DefaultStyle() const;
	const TextFont&  DefaultFont() const;

	int LineHeight() const   { return m_lineHeight; }
	int AvgCharWidth() const { return m_avgCharWidth; }
	int TabWidth() const     { return m_theme.tabWidth * m_avgCharWidth; }

	uint32_t AddFallbackFont( const std::wstring& name );
	void RemoveFallbackFont( uint32_t fontID );
	const std::set<uint32_t>& FallbackFonts() const;

private:
	void UpdateFontMetrics();
	void ResetFallbackFonts();

	TextTheme m_theme;
	TextAnnotator* m_annotator;

	int m_lineHeight;
	int m_avgCharWidth;

	uint32_t m_nextClassID;
	typedef std::map<std::string,uint32_t> ClassMap;
	ClassMap m_classes;

	typedef std::map<uint32_t,TextStyle> StyleMap;
	StyleMap m_styles;

	TextStyle m_defaultStyle;

	std::set<uint32_t> m_fallbackFonts;
};

#endif
