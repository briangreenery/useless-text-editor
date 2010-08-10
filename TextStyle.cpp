// TextStyle.cpp

#include "TextStyle.h"
#include "Error.h"

namespace W = Windows;

TextStyle::TextStyle()
{
	CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_ALL, IID_IMLangFontLink2, reinterpret_cast<void**>( &fontLink ) );

	LOGFONT logFont = {};
	logFont.lfHeight = -50;
	wcscpy_s( logFont.lfFaceName, L"Consolas" );

	HFONT font = CreateFontIndirect( &logFont );

	HDC hdc = GetDC( NULL );
	SelectObject( hdc, font );

	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	DWORD codePages = 0;
	if ( fontLink )
		fontLink->GetFontCodePages( hdc, font, &codePages );

	ReleaseDC( NULL, hdc );

	fonts.push_back( TextFont( font, codePages ) );

	gutterBrush    = CreateSolidBrush( RGB( 236, 236, 236 ) );
	selectionBrush = CreateSolidBrush( RGB( 173, 214, 255 ) );

	lineHeight = tm.tmHeight;
	avgCharWidth = tm.tmAveCharWidth;
	tabSize = 4 * tm.tmAveCharWidth;
}

int TextStyle::FontFallback( HDC hdc, int style, UTF16Ref text )
{
	if ( !fontLink )
		return style;

	DWORD codePages;
	long count;

	if ( FAILED( fontLink->GetStrCodePages( text.begin(), text.size(), fonts[style].codePages, &codePages, &count ) ) )
		return style;

	for ( size_t i = 0; i < fonts.size(); ++i )
	{
		if ( codePages & fonts[i].codePages )
		{
			SelectObject( hdc, fonts[i].font );
			return i;
		}
	}

	SelectObject( hdc, fonts[style].font );

	HFONT font;
	if ( FAILED( fontLink->MapFont( hdc, codePages, 0, &font ) ) )
		return style;

	fonts.push_back( TextFont( font, codePages ) );
	return fonts.size() - 1;
}

bool TextStyle::HasMissingGlyphs( int style, ArrayOf<WORD> glyphs ) const
{
	return false;
}
