// SimpleLayout.cpp

#include "SimpleLayout.h"
#include "SimpleLayoutData.h"
#include "SimpleTextRunLoop.h"
#include "SimpleTextBlock.h"
#include "TextDocument.h"
#include "TextStyle.h"
#include "Assert.h"

class DebuggableException {};
class SimpleLayoutFailed : public DebuggableException {};

static bool LayoutRun( SimpleTextRun& run,
                       int xStart,
                       UTF16Ref text,
                       SimpleLayoutData& layoutData,
                       TextStyle& style,
                       HDC hdc,
                       int maxWidth )
{
	SIZE size;
	INT fit;

	Assert( layoutData.xOffsets.size() == run.textStart );
	layoutData.xOffsets.resize( layoutData.xOffsets.size() + run.textCount );

	if ( run.textCount == 1 && text[run.textStart] == '\t' && style.tabSize > 0 )
	{
		int tabWidth = style.tabSize - ( xStart % style.tabSize );
		layoutData.xOffsets.back() = tabWidth;
		fit = ( tabWidth < maxWidth ? 1 : 0 );
	}
	else
	{
		SelectObject( hdc, style.fonts[run.font].font );
		if ( !GetTextExtentExPoint( hdc,
		                            text.begin() + run.textStart,
		                            run.textCount,
		                            ( maxWidth == 0 ? std::numeric_limits<int>::max() : maxWidth ),
		                            &fit,
		                            &layoutData.xOffsets.front() + run.textStart,
		                            &size ) )
		{
			Assert( false );
			throw SimpleLayoutFailed();
		}
	}

	bool wrapLine = ( size_t( fit ) < run.textCount );

	run.textCount = fit;
	layoutData.xOffsets.resize( run.textStart + fit );

	return wrapLine;
}

TextBlockPtr SimpleLayoutParagraph( UTF16Ref text,
                                    const TextDocument& doc,
                                    TextStyle& style,
                                    HDC hdc,
                                    int maxWidth,
                                    bool endsWithNewline )
{
	SimpleLayoutDataPtr layoutData( new SimpleLayoutData( text, endsWithNewline ) );

	int    lineWidth = 0;
	size_t lineStart = 0;

	for ( SimpleTextRunLoop loop( text ); loop.Unfinished(); )
	{
		SimpleTextRun run = loop.NextRun();

		bool wrapLine = LayoutRun( run, lineWidth, text, *layoutData, style, hdc, maxWidth - lineWidth );
		layoutData->runs.push_back( run );

		if ( wrapLine )
		{
			size_t lineEnd = run.textStart + run.textCount;

			size_t softBreak = doc.PrevSoftBreak( lineEnd + 1 );
			lineEnd = ( softBreak <= lineStart ) ? lineEnd : softBreak;

			layoutData->DiscardFrom( lineEnd );

			if ( lineStart == lineEnd )
			{
				run.textCount = 1;
				LayoutRun( run, lineWidth, text, *layoutData, style, hdc, 0 );
				layoutData->runs.push_back( run );
			}

			layoutData->lines.push_back( layoutData->runs.size() );

			lineWidth = 0;
			lineStart = lineEnd;

			loop.NewLine( lineStart );
		}
		else
		{
			lineWidth += layoutData->xOffsets.back();
		}
	}

	layoutData->lines.push_back( layoutData->runs.size() );
	return TextBlockPtr( new SimpleTextBlock( std::move( layoutData ), style ) );
}
