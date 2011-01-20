// SimpleLayout.cpp

#include "SimpleLayout.h"
#include "SimpleLayoutData.h"
#include "SimpleTextRunLoop.h"
#include "SimpleTextBlock.h"
#include "TextLayoutArgs.h"
#include "TextDocument.h"
#include "TextStyleRegistry.h"
#include "Assert.h"
#include <algorithm>

#undef min
#undef max

class DebuggableException {};
class SimpleLayoutFailed : public DebuggableException {};

static bool LayoutRun( SimpleTextRun run,
                       SimpleLayoutData& layoutData,
                       const TextLayoutArgs& args,
                       int xStart,
                       bool forceFit )
{
	SIZE size;
	INT fit;

	int maxWidth = ( forceFit || args.maxWidth == 0 ) ? std::numeric_limits<int>::max() : args.maxWidth;

	Assert( layoutData.xOffsets.size() == run.textStart );
	layoutData.xOffsets.resize( layoutData.xOffsets.size() + run.textCount );

	if ( run.textCount == 1 && args.text[run.textStart] == '\t' && args.styleRegistry.tabSize > 0 )
	{
		int tabWidth = args.styleRegistry.tabSize - ( xStart % args.styleRegistry.tabSize );
		layoutData.xOffsets.back() = tabWidth;
		fit = ( tabWidth < maxWidth ? 1 : 0 );
	}
	else
	{
		size_t textCount = std::min<size_t>( run.textCount, 1024 );
		
		SelectObject( args.hdc, args.styleRegistry.Font( run.fontid ).hfont );
		if ( !GetTextExtentExPoint( args.hdc,
		                            args.text.begin() + run.textStart,
		                            textCount,
		                            args.maxWidth - std::min( maxWidth, xStart ),
		                            &fit,
		                            &layoutData.xOffsets.front() + run.textStart,
		                            &size ) )
		{
			Assert( false );
			throw SimpleLayoutFailed();
		}
	}

	bool needsWrapping = ( size_t( fit ) < run.textCount );

	run.textCount = fit;
	layoutData.xOffsets.resize( run.textStart + fit );

	layoutData.runs.push_back( run );
	return !needsWrapping;
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

		if ( LayoutRun( run, *layoutData, args, lineWidth, false ) )
		{
			lineWidth += layoutData->xOffsets.back();
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
