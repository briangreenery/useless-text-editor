// UniscribeLayout.cpp

#include "UniscribeLayout.h"
#include "UniscribeLayoutData.h"
#include "UniscribeTextRunLoop.h"
#include "UniscribeTextBlock.h"
#include "TextLayoutArgs.h"
#include "TextStyleRegistry.h"
#include "UTF16Ref.h"
#include "ThrowHRESULT.h"
#include "CharBuffer.h"
#include <algorithm>

static std::vector<SCRIPT_ITEM> Itemize( UTF16Ref text )
{
	std::vector<SCRIPT_ITEM> items( text.size() / 4 + 2 );

	int numItems = 0;

	SCRIPT_CONTROL scriptControl = {};
	SCRIPT_STATE   scriptState   = {};

	scriptControl.uDefaultLanguage = LANG_USER_DEFAULT;

	while ( true )
	{
		HRESULT result = ScriptItemize( text.begin(),
		                                text.size(),
		                                items.size(),
		                                &scriptControl,
		                                &scriptState,
		                                &items.front(),
		                                &numItems );

		if ( result == S_OK )
			break;

		if ( result == E_OUTOFMEMORY )
			items.resize( items.size() * 2 );
		else
			ThrowHRESULT( result );
	}

	items.resize( numItems + 1 );
	return items;
}

static bool HasMissingGlyphs( uint32_t fontid, const std::vector<WORD>& glyphs, TextStyleRegistry& styleRegistry )
{
	WORD missingGlyph = styleRegistry.Font( fontid ).fontProps.wgDefault;
	return std::find( glyphs.begin(), glyphs.end(), missingGlyph ) != glyphs.end();
}

static bool ShapePlaceRun( UniscribeTextRun run,
                           UniscribeLayoutData& layoutData,
                           const TextLayoutArgs& args,
                           int xStart,
                           bool ignoreMissingGlyphs )
{
	size_t estimatedGlyphCount = MulDiv( 3, run.textCount, 2 ) + 16;

	std::vector<WORD>           logClusters( run.textCount );
	std::vector<WORD>           glyphs     ( estimatedGlyphCount );
	std::vector<SCRIPT_VISATTR> visAttrs   ( estimatedGlyphCount );

	HDC hdc = 0;
	int glyphCount = 0;

	while ( true )
	{
		HRESULT result = ScriptShape( hdc,
		                              &args.styleRegistry.Font( run.fontid ).fontCache,
		                              args.text.begin() + run.textStart,
		                              run.textCount,
		                              glyphs.size(),
		                              &layoutData.items[run.item].a,
		                              &glyphs.front(),
		                              &logClusters.front(),
		                              &visAttrs.front(),
		                              &glyphCount );

		if ( result == S_OK )
			break;

		switch ( result )
		{
		case E_PENDING:
			SelectObject( args.hdc, args.styleRegistry.Font( run.fontid ).hfont );
			hdc = args.hdc;
			break;

		case E_OUTOFMEMORY:
			glyphs  .resize( glyphs  .size() + run.textCount );
			visAttrs.resize( visAttrs.size() + run.textCount );
			break;

		case USP_E_SCRIPT_NOT_IN_FONT:
			return false;

		default:
			ThrowHRESULT( result );
		}
	}

	glyphs  .resize( glyphCount );
	visAttrs.resize( glyphCount );

	if ( !ignoreMissingGlyphs && HasMissingGlyphs( run.fontid, glyphs, args.styleRegistry ) )
		return false;

	std::vector<int>     advanceWidths( glyphCount );
	std::vector<GOFFSET> offsets      ( glyphCount );

	ABC abc;

	while ( true )
	{
		HRESULT result = ScriptPlace( hdc,
		                              &args.styleRegistry.Font( run.fontid ).fontCache,
		                              &glyphs.front(),
		                              glyphs.size(),
		                              &visAttrs.front(),
		                              &layoutData.items[run.item].a,
		                              &advanceWidths.front(),
		                              &offsets.front(),
		                              &abc );

		if ( result == S_OK )
			break;

		if ( result == E_PENDING )
		{
			SelectObject( args.hdc, args.styleRegistry.Font( run.fontid ).hfont );
			hdc = args.hdc;
		}
		else
		{
			ThrowHRESULT( result );
		}
	}

	run.glyphStart = layoutData.glyphs.size();
	run.glyphCount = glyphCount;

	if ( run.textCount == 1 && args.text[run.textStart] == '\t' && args.styleRegistry.TabWidth() > 0 )
	{
		run.width        = args.styleRegistry.TabWidth() - ( xStart % args.styleRegistry.TabWidth() );
		advanceWidths[0] = run.width;
	}
	else
	{
		run.width = abc.abcA + abc.abcB + abc.abcC;
	}

	layoutData.AddRun( run, logClusters, glyphs, visAttrs, advanceWidths, offsets );
	return true;
}

struct TryEveryFontProcData
{
	TryEveryFontProcData( UniscribeTextRun _run,
	                      UniscribeLayoutData& _layoutData,
	                      const TextLayoutArgs& _args,
	                      int _xStart )
		: run( _run )
		, layoutData( _layoutData )
		, args( _args )
		, xStart( _xStart )
	{}

	UniscribeTextRun run;
	UniscribeLayoutData& layoutData;
	const TextLayoutArgs& args;
	int xStart;
};

static int CALLBACK TryEveryFontProc( const LOGFONT* logFont, const TEXTMETRIC*, DWORD fontType, LPARAM lParam )
{
	if ( fontType != TRUETYPE_FONTTYPE )
		return 1;

	TryEveryFontProcData& data = *reinterpret_cast<TryEveryFontProcData*>( lParam );

	data.run.fontid = data.args.styleRegistry.AddFallbackFont( logFont->lfFaceName );

	if ( ShapePlaceRun( data.run, data.layoutData, data.args, data.xStart, false ) )
		return 0;

	data.args.styleRegistry.RemoveFallbackFont( data.run.fontid );
	return 1;
}

static void LayoutRun( UniscribeTextRun run,
                       UniscribeLayoutData& layoutData,
                       const TextLayoutArgs& args,
                       int xStart )
{
	if ( ShapePlaceRun( run, layoutData, args, xStart, false ) )
		return;

	// Try to fallback with the fonts that we already have
	for ( auto it = args.styleRegistry.FallbackFonts().begin(); it != args.styleRegistry.FallbackFonts().end(); ++it )
	{
		run.fontid = *it;
		if ( ShapePlaceRun( run, layoutData, args, xStart, false ) )
			return;
	}

	// Try to fallback on all fonts in the system
	TryEveryFontProcData data( run, layoutData, args, xStart );
	LOGFONT logFont = {};
	if ( EnumFontFamiliesEx( args.hdc, &logFont, TryEveryFontProc, reinterpret_cast<LPARAM>( &data ), 0 ) == 0 )
		return;

	// We can't display this text with any font, just show missing glyphs
	run.fontid = args.styleRegistry.DefaultStyle().fontid;
	layoutData.items[run.item].a.eScript = SCRIPT_UNDEFINED;
	ShapePlaceRun( run, layoutData, args, xStart, true );
}

static size_t EstimateLineWrap( UniscribeLayoutData& layoutData,
                                const TextLayoutArgs& args,
                                int lineWidth )
{
	const UniscribeTextRun& run = layoutData.runs.back();

	std::vector<int> logWidths( run.textCount );
	ThrowHRESULT( ScriptGetLogicalWidths( &layoutData.items[run.item].a,
	                                      run.textCount,
	                                      run.glyphCount,
	                                      &layoutData.advanceWidths.front() + run.glyphStart,
	                                      &layoutData.logClusters  .front() + run.textStart,
	                                      &layoutData.visAttrs     .front() + run.glyphStart,
	                                      &logWidths.front() ) );

	size_t estimate = 0;
	while ( estimate < logWidths.size() && lineWidth + logWidths[estimate] <= args.maxWidth )
	{
		lineWidth += logWidths[estimate];
		estimate++;
	}

	return run.textStart + estimate;
}

static size_t WrapLine( UniscribeLayoutData& layoutData,
                        const TextLayoutArgs& args,
                        size_t lineStart,
                        int lineWidth )
{
	size_t estimate = args.textStart + EstimateLineWrap( layoutData, args, lineWidth );

	size_t lineEnd = args.charBuffer.PrevSoftBreak( estimate + 1 );

	if ( lineEnd <= args.textStart + lineStart )
		lineEnd = args.charBuffer.PrevWordStop( estimate + 1 );

	if ( lineEnd <= args.textStart + lineStart )
		lineEnd = args.charBuffer.PrevCharStop( estimate + 1 );

	if ( lineEnd <= args.textStart + lineStart )
		lineEnd = (std::max)( estimate, args.textStart + lineStart + 1 );

	lineEnd -= args.textStart;

	UniscribeTextRun fragment = layoutData.DiscardFrom( lineEnd );

	if ( fragment.textCount > 0 )
		LayoutRun( fragment, layoutData, args, lineWidth );

	return lineEnd;
}

TextBlockPtr UniscribeLayoutParagraph( const TextLayoutArgs& args )
{
	UniscribeLayoutDataPtr layoutData( new UniscribeLayoutData( args.text, args.endsWithNewline ) );

	layoutData->items = Itemize( args.text );

	int    lineWidth = 0;
	size_t lineStart = 0;

	for ( UniscribeTextRunLoop loop( args.text, layoutData->items, args.fonts ); loop.Unfinished(); )
	{
		UniscribeTextRun run = loop.NextRun();

		LayoutRun( run, *layoutData, args, lineWidth );
		int runWidth = layoutData->runs.back().width;

		if ( args.maxWidth == 0 || lineWidth + runWidth <= args.maxWidth )
		{
			lineWidth += runWidth;
		}
		else
		{
			lineStart = WrapLine( *layoutData, args, lineStart, lineWidth );
			lineWidth = 0;

			layoutData->lines.push_back( layoutData->runs.size() );
			loop.NewLine( lineStart );
		}
	}

	if ( layoutData->lines.empty() || layoutData->lines.back() != layoutData->runs.size() )
		layoutData->lines.push_back( layoutData->runs.size() );

	return TextBlockPtr( new UniscribeTextBlock( std::move( layoutData ), args.styleRegistry ) );
}
