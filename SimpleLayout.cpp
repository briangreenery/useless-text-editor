// SimpleLayout.cpp

#include "SimpleLayout.h"
#include "SimpleLayoutData.h"
#include "SimpleTextRunLoop.h"
#include "SimpleTextBlock.h"
#include "TextLayoutArgs.h"
#include "TextDocument.h"
#include "TextStyleRegistry.h"
#include <cassert>
#include <algorithm>

class SimpleLayoutFailed {};

static size_t LayoutRun( SimpleTextRun run,
                         SimpleLayoutData& layoutData,
                         const TextLayoutArgs& args,
                         int xStart,
                         bool forceFit )
{
	SIZE size;
	INT fit;

	int maxWidth = ( forceFit || args.maxWidth == 0 ) ? (std::numeric_limits<int>::max)() : args.maxWidth;

	if ( run.textCount == 1 && args.text[run.textStart] == '\t' && args.styleRegistry.TabWidth() > 0 )
	{
		int tabWidth = args.styleRegistry.TabWidth() - ( xStart % args.styleRegistry.TabWidth() );
		layoutData.xOffsets[run.textStart] = tabWidth;
		fit = ( tabWidth < maxWidth ? 1 : 0 );
	}
	else
	{
		size_t textCount = std::min<size_t>( run.textCount, 256 );
		
		SelectObject( args.hdc, args.styleRegistry.Font( run.fontid ).hfont );
		if ( !GetTextExtentExPoint( args.hdc,
		                            args.text.begin() + run.textStart,
		                            textCount,
		                            maxWidth - (std::min)( maxWidth, xStart ),
		                            &fit,
		                            &layoutData.xOffsets.front() + run.textStart,
		                            &size ) )
		{
			assert( false );
			throw SimpleLayoutFailed();
		}
	}

	run.textCount = fit;
	layoutData.runs.push_back( run );
	return fit;
}

static size_t WrapLine( SimpleLayoutData& layoutData,
                        const TextLayoutArgs& args,
                        size_t lineStart,
                        int lineWidth )
{
	SimpleTextRun lastRun = layoutData.runs.back();

	// 'lastRun' can have a textCount of 0
	size_t estimate = args.textStart + lastRun.textStart + lastRun.textCount;

	size_t lineEnd = args.doc.PrevSoftBreak( estimate + 1 );

	if ( lineEnd <= args.textStart + lineStart )
		lineEnd = args.doc.PrevWordStop( estimate + 1 );

	if ( lineEnd <= args.textStart + lineStart )
		lineEnd = args.doc.PrevCharStop( estimate + 1 );

	if ( lineEnd <= args.textStart + lineStart )
		lineEnd = estimate;

	lineEnd -= args.textStart;

	layoutData.DiscardFrom( lineEnd );

	if ( lineStart == lineEnd )
	{
		lastRun.textCount = 1;
		LayoutRun( lastRun, layoutData, args, lineWidth, true );
		lineEnd++;
	}

	return lineEnd;
}

TextBlockPtr SimpleLayoutParagraph( const TextLayoutArgs& args )
{
	SimpleLayoutDataPtr layoutData( new SimpleLayoutData( args.text, args.endsWithNewline ) );

	int    lineWidth = 0;
	size_t lineStart = 0;

	for ( SimpleTextRunLoop loop( args.text, args.fonts ); loop.Unfinished(); )
	{
		SimpleTextRun run = loop.NextRun();
		size_t fit = LayoutRun( run, *layoutData, args, lineWidth, false );

		if ( fit == run.textCount )
		{
			lineWidth += layoutData->xOffsets[run.textStart + run.textCount - 1];
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

	return TextBlockPtr( new SimpleTextBlock( std::move( layoutData ), args.styleRegistry ) );
}
