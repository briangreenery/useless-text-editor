// VisualDocument.h

#ifndef VisualDocument_h
#define VisualDocument_h

#include "VisualBlock.h"
#include "TextChange.h"
#include <Windows.h>
#include <list>

class TextDocument;
class TextSelection;
class TextStyle;

class VisualDocument
{
public:
	VisualDocument( const TextDocument&, TextStyle& );

	void Draw( HDC hdc, RECT rect, TextSelection );

	size_t LineCount() const;
	size_t LineStart( int y ) const;
	size_t LineEnd  ( int y ) const;

	int Height() const;

	POINT PointFromChar( size_t, bool advancing ) const;
	size_t CharFromPoint( POINT* ) const;

	void Update( HDC, int width, TextChange );
	void UpdateAll( HDC, int width );

private:
	void LayoutText( VisualBlockList::const_iterator, size_t start, size_t count, HDC hdc, int maxWidth );

	struct BlockContaining_Result
	{
		size_t textStart;
		int yStart;
		VisualBlockList::const_iterator it;

		const VisualBlock* operator->() const { return &*it; }
	};

	BlockContaining_Result BlockContaining( size_t pos ) const;
	BlockContaining_Result BlockContaining( int y ) const;

	const TextDocument& m_doc;
	TextStyle&          m_style;
	size_t              m_lineCount;
	VisualBlockList     m_blocks;
};

#endif
