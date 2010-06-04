// LayoutEngine.cpp

#include "LayoutEngine.h"
#include "TextRunLoop.h"
#include "LayoutData.h"
#include "Error.h"

namespace W = Windows;

LayoutEngine::LayoutEngine( HFONT font, SCRIPT_CACHE fontCache, HDC hdc, int maxWidth, int tabSize )
	: m_font( font )
	, m_fontCache( fontCache )
	, m_hdc( hdc )
	, m_maxWidth( maxWidth )
	, m_tabSize( tabSize )
{
}

int LayoutEngine::ShapePlaceRun( const UTF16::Unit* text, ArrayOf<SCRIPT_ITEM> items, TextRun* run, int xStart )
{
	size_t estimatedGlyphCount = m_allocator.EstimateGlyphCount( run->textCount );

	ArrayOf<WORD>           logClusters = m_allocator.logClusters.Alloc( run->textCount );
	ArrayOf<WORD>           glyphs      = m_allocator.glyphs     .Alloc( estimatedGlyphCount );
	ArrayOf<SCRIPT_VISATTR> visAttrs    = m_allocator.visAttrs   .Alloc( estimatedGlyphCount );

	HDC hdc        = 0;
	int glyphCount = 0;

	while ( true )
	{
		HRESULT result = ScriptShape( hdc,
		                              &m_fontCache,
		                              text + run->textStart,
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
			SelectObject( m_hdc, m_font );
			hdc = m_hdc;
			break;

		case E_OUTOFMEMORY:
			glyphs   = m_allocator.glyphs  .Grow( glyphs,   run->textCount );
			visAttrs = m_allocator.visAttrs.Grow( visAttrs, run->textCount );
			break;

		case USP_E_SCRIPT_NOT_IN_FONT:
			// IMLangFontLink2
			//item->style = styles.Fallback( UTF16Ref( text + item->textStart, item->textCount ),
			//							   hTargetDC );
			//style = styles[item->style];
			//break;

		default:
			W::ThrowHRESULT( result );
		}
	}

	glyphs   = m_allocator.glyphs  .Shrink( glyphs,   glyphCount );
	visAttrs = m_allocator.visAttrs.Shrink( visAttrs, glyphCount );

	// FIXME: Scan for missing glyphs to support east asian text.  Use ScriptGetFontProperties to determine the index of the missing glyph.

	ArrayOf<int>     advanceWidths = m_allocator.advanceWidths.Alloc( glyphCount );
	ArrayOf<GOFFSET> offsets       = m_allocator.offsets      .Alloc( glyphCount );

	ABC abc;

	while ( true )
	{
		HRESULT result = ScriptPlace( hdc,
		                              &m_fontCache,
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
			SelectObject( hdc, m_font );
			hdc = m_hdc;
			break;

		default:
			W::ThrowHRESULT( result );
		}
	}

	run->glyphStart = m_allocator.glyphs.MakeRelative( glyphs.begin() );
	run->glyphCount = glyphCount;

	if ( text[run->textStart] == '\t' && m_tabSize > 0 )
	{
		run->width       = m_tabSize - ( xStart % m_tabSize );
		advanceWidths[0] = run->width;
	}
	else
	{
		run->width = abc.abcA + abc.abcB + abc.abcC;
	}

	return run->width;
}

ArrayOf<SCRIPT_ITEM> LayoutEngine::Itemize( UTF16Ref text )
{
	ArrayOf<SCRIPT_ITEM> items = m_allocator.items.Alloc( text.size() + 1 );

	int numItems = 0;

	while ( true )
	{
		HRESULT result
			= ScriptItemize( text.begin(), text.size(), items.size(), NULL, NULL, items.begin(), &numItems );

		if ( result == S_OK )
			break;

		switch ( result )
		{
		case E_OUTOFMEMORY:
			items = m_allocator.items.Grow( items, text.size() );
			break;

		default:
			W::ThrowHRESULT( result );
		}
	}

	items = m_allocator.items.Shrink( items, numItems + 1 );
	return ArrayOf<SCRIPT_ITEM>( items.begin(), items.end() - 1 );
}

size_t LayoutEngine::EstimateLineWrap( const UTF16::Unit* text, ArrayOf<SCRIPT_ITEM> items, TextRun* run, int lineWidth )
{
	ArrayOf<int> logWidths = m_allocator.logWidths.TempArray( run->textCount );

	W::ThrowHRESULT( ScriptGetLogicalWidths( &items[run->item].a,
	                                         run->textCount,
	                                         run->glyphCount,
	                                         m_allocator.advanceWidths.MakeAbsolute( run->glyphStart ),
	                                         m_allocator.logClusters  .MakeAbsolute( run->textStart ),
	                                         m_allocator.visAttrs     .MakeAbsolute( run->glyphStart ),
	                                         logWidths.begin() ) );

	size_t estimate = 0;

	for ( ; estimate < logWidths.size(); ++estimate )
	{
		if ( lineWidth + logWidths[estimate] <= m_maxWidth )
			lineWidth += logWidths[estimate];
		else
			break;
	}

	return run->textStart + estimate;
}

size_t LayoutEngine::ForceLineWrap( const UTF16::Unit* text, ArrayOf<SCRIPT_ITEM> items, TextRun* run )
{
	ArrayOf<SCRIPT_LOGATTR> attrs = m_allocator.logAttr.TempArray( run->textCount );

	W::ThrowHRESULT( ScriptBreak( text + run->textStart, run->textCount, &items[run->item].a, attrs.begin() ) );

	for ( SCRIPT_LOGATTR* it = attrs.begin(); it != attrs.end(); ++it )
		if ( it->fCharStop || it->fWordStop || it->fWhiteSpace )
			return ( it - attrs.begin() ) + 1;

	return run->textStart + run->textCount;
}

size_t LayoutEngine::FindSoftBreak( const UTF16::Unit* text, SCRIPT_ITEM* item, size_t estimate )
{
	size_t textStart = item[0].iCharPos;
	size_t textSize  = item[1].iCharPos - item[0].iCharPos;

	ArrayOf<SCRIPT_LOGATTR> attrs = m_allocator.logAttr.TempArray( textSize );

	W::ThrowHRESULT( ScriptBreak( text + textStart, textSize, &item->a, attrs.begin() ) );

	Assert( estimate >= textStart );
	size_t offset = (std::min)( estimate - textStart + 1, textSize ) - 1;

	const SCRIPT_LOGATTR* attr  = attrs.begin()    + offset;
	const UTF16::Unit*    unit  = text + textStart + offset;
	const UTF16::Unit*    rend  = text + textStart - 1;

	for ( ; unit != rend; --unit, --attr )
	{
		if ( attr->fSoftBreak )
			return unit - text;

		if ( attr->fWhiteSpace || *unit == '\\' || *unit == '-' )
			return unit - text + 1;
	}

	return noSoftBreak;
}

TextRun* LayoutEngine::DiscardRunsAfter( size_t lineEnd )
{
	ArrayOf<TextRun> runs = m_allocator.runs.Allocated();
	Assert( !runs.empty() );

	TextRun* run = runs.end() - 1;

	while ( run->textStart > lineEnd )
		--run;

	m_allocator.DiscardFrom( run->textStart, run->glyphStart );

	if ( run->textStart == lineEnd )
	{
		m_allocator.runs.DiscardFrom( run );
		return 0;
	}

	m_allocator.runs.DiscardFrom( run + 1 );
	run->textCount = lineEnd - run->textStart;
	return run;
}

size_t LayoutEngine::WrapLine( const UTF16::Unit* text, ArrayOf<SCRIPT_ITEM> items, TextRun* run, size_t lineStart, int lineWidth )
{
	size_t lineEndEstimate = EstimateLineWrap( text, items, run, lineWidth );
	size_t lineEnd         = noSoftBreak;

	if ( lineEndEstimate > lineStart )
	{
		size_t estimate = lineEndEstimate;

		for ( SCRIPT_ITEM* item = items.begin() + run->item; item != items.begin() - 1; --item )
		{
			lineEnd  = FindSoftBreak( text, item, estimate );
			estimate = item->iCharPos;

			if ( lineEnd != noSoftBreak || estimate <= lineStart )
				break;
		}

		if ( lineEnd == noSoftBreak || lineEnd <= lineStart )
			lineEnd = lineEndEstimate;
	}
	else
	{
		lineEnd = ForceLineWrap( text, items, run );
	}

	TextRun* fragment = DiscardRunsAfter( lineEnd );

	if ( fragment )
		ShapePlaceRun( text, items, fragment, lineWidth );

	AddLine();
	return lineEnd;
}

void LayoutEngine::AddLine()
{
	m_allocator.lines.Alloc( 1 )[0] = m_allocator.runs.Allocated().size();
}

void LayoutEngine::LayoutParagraph( UTF16Ref text, ArrayOf<StyleRun> styleRuns )
{
	Assert( !text.empty() );
	Assert( !styleRuns.empty() );

	m_allocator.Reset( text.size() );

	ArrayOf<SCRIPT_ITEM> items = Itemize( text );

	int    lineWidth = 0;
	size_t lineStart = 0;

	for ( TextRunLoop loop( items, styleRuns, text, m_allocator ); loop.Unfinished(); )
	{
		TextRun* run = loop.NextRun();
		int runWidth = ShapePlaceRun( text.begin(), items, run, lineWidth );

		if ( lineWidth + runWidth <= m_maxWidth )
		{
			lineWidth += runWidth;
		}
		else
		{
			lineStart = WrapLine( text.begin(), items, run, lineStart, lineWidth );
			lineWidth = 0;
			loop.NewLine();
		}
	}

	AddLine();
}
