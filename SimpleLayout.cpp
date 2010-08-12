// SimpleLayout.cpp

#include "SimpleLayout.h"
#include "SimpleRunLoop.h"
#include "VectorAllocator.h"
#include "TextStyle.h"
#include "Assert.h"
//
//class SimpleLayoutFailed {};
//
//static size_t LayoutRun( SimpleRun& run, UTF16Ref text, VectorAllocator<INT>& allocator, TextStyle& style, HDC hdc, int maxWidth )
//{
//	SIZE size;
//	INT fit;
//
//	ArrayOf<INT> dx = allocator.Alloc( text.size() );
//
//	SelectObject( hdc, style.fonts[run.font].font );
//	if ( !GetTextExtentExPoint( hdc,
//	                            text.begin() + run.textStart,
//	                            run.textCount,
//	                            maxWidth,
//	                            &fit,
//	                            dx.begin(),
//	                            &size ) )
//	{
//		Assert( false );
//		throw SimpleLayoutFailed();
//	}
//
//	Assert( fit > 0 );
//
//	run.dxStart   = allocator.MakeRelative( dx.begin() );
//	run.dxCount   = fit;
//	run.textCount = fit;
//	run.width     = size.cx;
//
//	return fit;
//}
//
//TextBlockPtr SimpleLayoutParagraph( UTF16Ref text, TextStyle& style, HDC hdc, int maxWidth, bool endsWithNewline )
//{
//	VectorAllocator<INT> allocator;
//	
//	std::vector<size_t> lines;
//	std::vector<SimpleRun> runs;
//
//	int lineWidth = 0;
//
//	for ( SimpleRunLoop loop( text, style ); loop.Unfinished(); ++loop )
//	{
//		SimpleRun run = loop.NextRun();
//
//		size_t count = LayoutRun( run, text, allocator, style, hdc, maxWidth - lineWidth );
//		runs.push_back( run );
//
//		if ( count == run.textCount )
//		{
//			lineWidth += run.width;
//		}
//		else
//		{
//			loop.NewLine( count );
//			lineWidth = 0;
//
//			lines.push_back( runs.size() );
//		}
//	}
//
//	lines.push_back( runs.size() );
//
//	return TextBlockPtr( new SimpleTextBlock( lines, runs, allocator.Finish(), endsWithNewline ) );
//}
