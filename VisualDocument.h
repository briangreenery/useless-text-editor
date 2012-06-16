// VisualDocument.h

#ifndef VisualDocument_h
#define VisualDocument_h

#include "TextBlock.h"
#include "TextChange.h"
#include "UTF16Ref.h"
#include "TextSquiggle.h"
#include <Windows.h>
#include <list>

class TextDocument;
class TextSelection;
class TextStyleRegistry;

class VisualDocument
{
public:
	VisualDocument( const TextDocument&, TextStyleRegistry& );

	void Draw( HDC hdc, RECT rect, TextSelection ) const;

	size_t LineCount() const;
	size_t LineStart( int y ) const;
	size_t LineEnd  ( int y ) const;

	int Height() const;

	POINT PointFromChar( size_t, bool advancing ) const;
	size_t CharFromPoint( POINT* ) const;

	void Update( HDC, int width, TextChange );

private:
	bool IsSimpleText( UTF16Ref ) const;
	void LayoutText( TextBlockList::const_iterator, size_t start, size_t count, HDC hdc, int maxWidth );

	struct BlockContaining_Result
	{
		size_t textStart;
		int yStart;
		TextBlockList::const_iterator it;

		const TextBlock* operator->() const { return it->get(); }
	};

	BlockContaining_Result BlockContaining( size_t pos ) const;
	BlockContaining_Result BlockContaining( int y ) const;

	typedef void (TextBlock::*DrawFunction)( VisualPainter&, RECT ) const;
	void Draw( BlockContaining_Result, VisualPainter&, RECT, DrawFunction ) const;

	const TextDocument& m_doc;
	TextStyleRegistry&  m_styleRegistry;
	TextSquiggle        m_squiggle;
	size_t              m_lineCount;
	TextBlockList       m_blocks;
};

#endif
