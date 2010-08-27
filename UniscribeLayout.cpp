// UniscribeLayout.cpp

#include "UniscribeLayout.h"
#include "UniscribeRunLoop.h"
#include "UniscribeTextBlock.h"
#include "TextStyle.h"
#include "Error.h"

#undef min
#undef max

namespace W = Windows;

static void Itemize( UTF16Ref text, UniscribeAllocator& allocator )
{
	ArrayOf<SCRIPT_ITEM> items = allocator.items.Alloc( text.size() + 1 );

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
		                                items.begin(),
		                                &numItems );

		if ( result == S_OK )
			break;

		switch ( result )
		{
		case E_OUTOFMEMORY:
			items = allocator.items.Grow( items, text.size() );
			break;

		default:
			W::ThrowHRESULT( result );
		}
	}

	items = allocator.items.Shrink( items, numItems + 1 );
}

static bool HasMissingGlyphs( size_t font, ArrayOf<WORD> glyphs, TextStyle& style )
{
	WORD missingGlyph = style.fonts[font].fontProps.wgDefault;
	return std::find( glyphs.begin(), glyphs.end(), missingGlyph ) != glyphs.end();
}

static bool ShapePlaceRun( UniscribeRun* run, int xStart, UTF16Ref text, UniscribeAllocator& allocator, TextStyle& style, HDC hdc, bool forceMissingGlyphs )
{
	size_t estimatedGlyphCount = allocator.EstimateGlyphCount( run->textCount );

	ArrayOf<SCRIPT_ITEM>    items       = allocator.items      .Allocated();
	ArrayOf<WORD>           logClusters = allocator.logClusters.Alloc( run->textCount );
	ArrayOf<WORD>           glyphs      = allocator.glyphs     .Alloc( estimatedGlyphCount );
	ArrayOf<SCRIPT_VISATTR> visAttrs    = allocator.visAttrs   .Alloc( estimatedGlyphCount );

	HDC shapingDC  = 0;
	int glyphCount = 0;

	while ( true )
	{
		HRESULT result = ScriptShape( shapingDC,
		                              &style.fonts[run->font].fontCache,
		                              text.begin() + run->textStart,
		                              run->textCount,
		                              glyphs.size(),
		                              &items[run->item].a,
		                              glyphs.begin(),
		                              logClusters.begin(),
		                              visAttrs.begin(),
		                              &glyphCount );

		if ( result == S_OK )
			break;

		switch ( result )
		{
		case E_PENDING:
			SelectObject( hdc, style.fonts[run->font].font );
			shapingDC = hdc;
			break;

		case E_OUTOFMEMORY:
			glyphs   = allocator.glyphs  .Grow( glyphs,   run->textCount );
			visAttrs = allocator.visAttrs.Grow( visAttrs, run->textCount );
			break;

		case USP_E_SCRIPT_NOT_IN_FONT:
			allocator.logClusters.DiscardFrom( logClusters.begin() );
			allocator.glyphs     .DiscardFrom( glyphs.begin() );
			allocator.visAttrs   .DiscardFrom( visAttrs.begin() );
			return false;

		default:
			W::ThrowHRESULT( result );
		}
	}

	glyphs   = allocator.glyphs  .Shrink( glyphs,   glyphCount );
	visAttrs = allocator.visAttrs.Shrink( visAttrs, glyphCount );

	if ( !forceMissingGlyphs && HasMissingGlyphs( run->font, glyphs, style ) )
	{
		allocator.logClusters.DiscardFrom( logClusters.begin() );
		allocator.glyphs     .DiscardFrom( glyphs.begin() );
		allocator.visAttrs   .DiscardFrom( visAttrs.begin() );
		return false;
	}

	ArrayOf<int>     advanceWidths = allocator.advanceWidths.Alloc( glyphCount );
	ArrayOf<GOFFSET> offsets       = allocator.offsets      .Alloc( glyphCount );

	ABC abc;

	while ( true )
	{
		HRESULT result = ScriptPlace( shapingDC,
		                              &style.fonts[run->font].fontCache,
		                              glyphs.begin(),
		                              glyphs.size(),
		                              visAttrs.begin(),
		                              &items[run->item].a,
		                              advanceWidths.begin(),
		                              offsets.begin(),
		                              &abc );

		if ( result == S_OK )
			break;

		switch ( result )
		{
		case E_PENDING:
			SelectObject( hdc, style.fonts[run->font].font );
			shapingDC = hdc;
			break;

		default:
			W::ThrowHRESULT( result );
		}
	}

	run->glyphStart = allocator.glyphs.MakeRelative( glyphs.begin() );
	run->glyphCount = glyphCount;

	if ( text[run->textStart] == '\t' && style.tabSize > 0 )
	{
		run->width       = style.tabSize - ( xStart % style.tabSize );
		advanceWidths[0] = run->width;
	}
	else
	{
		run->width = abc.abcA + abc.abcB + abc.abcC;
	}

	return true;
}

struct EnumFontData
{
	EnumFontData( UniscribeRun* _run, int _xStart, UTF16Ref _text, UniscribeAllocator& _allocator, TextStyle& _style, HDC _hdc )
		: run( _run )
		, xStart( _xStart )
		, text( _text )
		, allocator( _allocator )
		, style( _style )
		, hdc( _hdc )
		, deleteLastFont( false )
	{}

	UniscribeRun* run;
	int xStart;
	UTF16Ref text;
	UniscribeAllocator& allocator;
	TextStyle& style;
	HDC hdc;
	bool deleteLastFont;
};

static int CALLBACK TryEveryFontProc( const LOGFONT* logFont, const TEXTMETRIC*, DWORD fontType, LPARAM lParam )
{
	if ( fontType != TRUETYPE_FONTTYPE )
		return 1;

	EnumFontData& data = *reinterpret_cast<EnumFontData*>( lParam );

	if ( data.deleteLastFont )
		data.style.DeleteLastFont();

	size_t numFonts = data.style.fonts.size();
	data.run->font = data.style.AddFont( logFont->lfFaceName );
	data.deleteLastFont = ( data.run->font == numFonts );

	if ( ShapePlaceRun( data.run, data.xStart, data.text, data.allocator, data.style, data.hdc, false ) )
		return 0;

	return 1;
}

static void LayoutRun( UniscribeRun* run, int xStart, UTF16Ref text, UniscribeAllocator& allocator, TextStyle& style, HDC hdc )
{
	if ( ShapePlaceRun( run, xStart, text, allocator, style, hdc, false ) )
		return;

	// Try to fallback with the fonts that we already have
	run->font = 0;
	while ( run->font < style.fonts.size() && !ShapePlaceRun( run, xStart, text, allocator, style, hdc, false ) )
		++run->font;

	if ( run->font < style.fonts.size() )
		return;

	// Try to fallback on all truetype fonts in the system
	EnumFontData data( run, xStart, text, allocator, style, hdc );
	LOGFONT logFont = {};
	if ( EnumFontFamiliesEx( hdc, &logFont, TryEveryFontProc, reinterpret_cast<LPARAM>( &data ), 0 ) == 0 )
		return;

	if ( data.deleteLastFont )
		data.style.DeleteLastFont();

	// We can't display this text with any font, just show missing glyphs
	run->font = style.defaultFont;

	ArrayOf<SCRIPT_ITEM> items = allocator.items.Allocated();

	items[run->item].a.eScript = SCRIPT_UNDEFINED;
	ShapePlaceRun( run, xStart, text, allocator, style, hdc, true );
}

static size_t EstimateLineWrap( UniscribeRun* run, int lineWidth, UTF16Ref text, UniscribeAllocator& allocator, int maxWidth )
{
	ArrayOf<SCRIPT_ITEM> items     = allocator.items.Allocated();
	ArrayOf<int>         logWidths = allocator.logWidths.TempArray( run->textCount );

	W::ThrowHRESULT( ScriptGetLogicalWidths( &items[run->item].a,
	                                         run->textCount,
	                                         run->glyphCount,
	                                         allocator.advanceWidths.MakeAbsolute( run->glyphStart ),
	                                         allocator.logClusters  .MakeAbsolute( run->textStart ),
	                                         allocator.visAttrs     .MakeAbsolute( run->glyphStart ),
	                                         logWidths.begin() ) );

	size_t estimate = 0;

	for ( ; estimate < logWidths.size(); ++estimate )
	{
		if ( lineWidth + logWidths[estimate] > maxWidth )
			break;

		lineWidth += logWidths[estimate];
	}

	return run->textStart + estimate;
}

static size_t ForceLineWrap( UniscribeRun* run, UTF16Ref text, UniscribeAllocator& allocator )
{
	ArrayOf<SCRIPT_ITEM>    items = allocator.items.Allocated();
	ArrayOf<SCRIPT_LOGATTR> attrs = allocator.logAttr.TempArray( run->textCount );

	W::ThrowHRESULT( ScriptBreak( text.begin() + run->textStart,
	                              run->textCount,
	                              &items[run->item].a,
	                              attrs.begin() ) );

	for ( SCRIPT_LOGATTR* it = attrs.begin(); it != attrs.end(); ++it )
		if ( it->fCharStop || it->fWordStop || it->fWhiteSpace )
			return ( it - attrs.begin() ) + 1;

	return run->textStart + run->textCount;
}

static const size_t noSoftBreak = std::numeric_limits<size_t>::max();

static size_t FindSoftBreak( SCRIPT_ITEM* item, size_t estimate, UTF16Ref text, UniscribeAllocator& allocator )
{
	size_t textStart = item[0].iCharPos;
	size_t textSize  = item[1].iCharPos - item[0].iCharPos;

	ArrayOf<SCRIPT_LOGATTR> attrs = allocator.logAttr.TempArray( textSize );

	W::ThrowHRESULT( ScriptBreak( text.begin() + textStart,
	                              textSize,
	                              &item->a,
	                              attrs.begin() ) );

	Assert( estimate >= textStart );
	size_t offset = std::min( estimate - textStart + 1, textSize ) - 1;

	const SCRIPT_LOGATTR* attr  = attrs.begin() + offset;
	const UTF16::Unit*    unit  = text.begin() + textStart + offset;
	const UTF16::Unit*    rend  = text.begin() + textStart - 1;

	for ( ; unit != rend; --unit, --attr )
	{
		if ( attr->fSoftBreak )
			return unit - text.begin();

		if ( attr->fWhiteSpace || *unit == '\\' || *unit == '-' )
			return unit - text.begin() + 1;
	}

	return noSoftBreak;
}

static UniscribeRun* DiscardRunsAfter( size_t lineEnd, UniscribeAllocator& allocator )
{
	UniscribeRun* run = allocator.runs.Allocated().end() - 1;

	while ( run->textStart > lineEnd )
		--run;

	allocator.DiscardFrom( run->textStart, run->glyphStart );

	if ( run->textStart == lineEnd )
	{
		allocator.runs.DiscardFrom( run );
		return 0;
	}

	allocator.runs.DiscardFrom( run + 1 );
	run->textCount = lineEnd - run->textStart;
	return run;
}

static size_t WrapLine( UniscribeRun* run, size_t lineStart, int lineWidth, UTF16Ref text, UniscribeAllocator& allocator, TextStyle& style, HDC hdc, int maxWidth )
{
	size_t lineEndEstimate = EstimateLineWrap( run, lineWidth, text, allocator, maxWidth );
	size_t lineEnd         = noSoftBreak;

	if ( lineEndEstimate > lineStart )
	{
		ArrayOf<SCRIPT_ITEM> items = allocator.items.Allocated();

		size_t estimate = lineEndEstimate;

		for ( SCRIPT_ITEM* item = items.begin() + run->item; item != items.begin() - 1; --item )
		{
			lineEnd  = FindSoftBreak( item, estimate, text, allocator );
			estimate = item->iCharPos;

			if ( lineEnd != noSoftBreak || estimate <= lineStart )
				break;
		}

		if ( lineEnd == noSoftBreak || lineEnd <= lineStart )
			lineEnd = lineEndEstimate;
	}
	else
	{
		lineEnd = ForceLineWrap( run, text, allocator );
	}

	UniscribeRun* fragment = DiscardRunsAfter( lineEnd, allocator );

	if ( fragment )
		LayoutRun( fragment, lineWidth, text, allocator, style, hdc );

	allocator.lines.PushBack( allocator.runs.Allocated().size() );
	return lineEnd;
}

TextBlockPtr UniscribeLayoutParagraph( UTF16Ref text,
                                       const TextDocument&,
                                       TextStyle& style,
                                       HDC hdc,
                                       int maxWidth,
                                       bool endsWithNewline )
{
	UniscribeAllocator allocator;

	Itemize( text, allocator );

	int    lineWidth = 0;
	size_t lineStart = 0;

	for ( UniscribeRunLoop loop( text, allocator ); loop.Unfinished(); )
	{
		UniscribeRun* run = loop.NextRun();

		LayoutRun( run, lineWidth, text, allocator, style, hdc );

		if ( maxWidth == 0 || lineWidth + run->width <= maxWidth )
		{
			lineWidth += run->width;
		}
		else
		{
			lineStart = WrapLine( run, lineStart, lineWidth, text, allocator, style, hdc, maxWidth );
			lineWidth = 0;

			loop.NewLine();
		}
	}

	ArrayOf<UniscribeRun> runs  = allocator.runs .Allocated();
	ArrayOf<size_t>       lines = allocator.lines.Allocated();

	if ( lines.empty() || runs.size() > lines[lines.size() - 1] )
		allocator.lines.PushBack( runs.size() );

	return TextBlockPtr( new UniscribeTextBlock( allocator, style, endsWithNewline ) );
}
