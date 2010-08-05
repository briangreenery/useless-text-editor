// LayoutEngine.h

#ifndef LayoutEngine_h
#define LayoutEngine_h

#include "LayoutAllocator.h"
#include "VisualBlock.h"
#include "DocumentReader.h"
#include "ArrayOf.h"
#include "UString.h"
#include <Windows.h>

#undef min
#undef max

class TextStyle;
class TextDocument;
class TextRunLoop;

class LayoutEngine
{
public:
	LayoutEngine( const TextDocument&, TextStyle&, HDC hdc, LONG maxWidth );

	VisualBlockList& LayoutText( size_t start, size_t count );

private:
	void LayoutBlocks( UTF16Ref, bool endsWithNewline );

	ArrayOf<SCRIPT_ITEM> Itemize( UTF16Ref );

	int ShapePlaceRun( const UTF16::Unit*, SCRIPT_ITEM*, TextRun*, int xStart );

	size_t EstimateLineWrap( const UTF16::Unit*, SCRIPT_ITEM*, TextRun*, int lineWidth );

	size_t ForceLineWrap( const UTF16::Unit*, SCRIPT_ITEM*, TextRun* );

	size_t FindSoftBreak( const UTF16::Unit*, SCRIPT_ITEM*, size_t blockStart, size_t estimate );

	size_t WrapLine( const UTF16::Unit*, SCRIPT_ITEM*, TextRun*, size_t blockStart, size_t lineStart, int lineWidth );

	TextRun* DiscardRunsAfter( size_t lineEnd );

	void AddLine( TextRunLoop& );
	void AddBlock( TextRunLoop& );
	void FinishBlock( TextRunLoop&, bool endsWithNewline );

	const TextDocument& m_doc;
	TextStyle&          m_style;
	HDC                 m_hdc;
	LONG                m_maxWidth;

	LayoutAllocator m_allocator;
	DocumentReader  m_reader;
	VisualBlockList m_result;

	static const size_t noSoftBreak = 0xffffffff;
};

#endif
