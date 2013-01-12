// VisualDocument.h

#ifndef VisualDocument_h
#define VisualDocument_h

#include "ArrayRef.h"
#include "TextBlock.h"
#include "CharChange.h"
#include "TextSquiggle.h"
#include <Windows.h>
#include <list>

class Document;
class CharSelection;
class TextStyleRegistry;

class VisualDocument
{
public:
	VisualDocument( const Document&, TextStyleRegistry& );

	void DrawText( HDC hdc, RECT rect, CharSelection ) const;
	void DrawLineNumbers( HDC hdc, RECT rect ) const;

	size_t LineCount() const;
	size_t LineStart( int y ) const;
	size_t LineEnd  ( int y ) const;

	int Height() const;

	POINT PointFromChar( size_t, bool advancing ) const;
	size_t CharFromPoint( POINT* ) const;

	void Update( HDC, int width, CharChange );

private:
	bool IsSimpleText( ArrayRef<const wchar_t> ) const;
	void LayoutText( TextBlockList::const_iterator, size_t start, size_t count, HDC hdc, int maxWidth );

	struct BlockContaining_Result
	{
		size_t textStart;
		size_t logicalLine;
		int yStart;
		TextBlockList::const_iterator it;

		const TextBlock* operator->() const { return it->get(); }
	};

	BlockContaining_Result BlockContaining( size_t pos ) const;
	BlockContaining_Result BlockContaining( int y ) const;

	typedef void (TextBlock::*DrawFunction)( VisualPainter&, RECT ) const;
	void Draw( BlockContaining_Result, VisualPainter&, RECT, DrawFunction ) const;

	const Document& m_doc;
	TextStyleRegistry&  m_styleRegistry;
	TextSquiggle        m_squiggle;
	size_t              m_lineCount;
	TextBlockList       m_blocks;
};

#endif
