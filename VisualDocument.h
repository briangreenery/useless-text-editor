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
class LayoutEngine;

class VisualDocument
{
public:
	VisualDocument( const TextDocument&, TextStyle& );

	void Draw( HDC hdc, RECT rect, TextSelection );

	size_t LineCount() const;
	size_t LineStart( LONG y ) const;
	size_t LineEnd  ( LONG y ) const;

	int Height() const;

	POINT PointFromChar( size_t, bool advancing ) const;
	size_t CharFromPoint( POINT* ) const;

	void Update( HDC, int width, TextChange );
	void UpdateAll( HDC, int width );

private:
	void TextInserted( size_t start, size_t count );
	void TextDeleted ( size_t start, size_t count );

	struct BlockContaining_Result
	{
		size_t textStart;
		LONG yStart;
		VisualBlockList::const_iterator it;

		const VisualBlock* operator->() const { return &*it; }
	};

	BlockContaining_Result BlockContaining( size_t pos ) const;
	BlockContaining_Result BlockContaining( LONG y ) const;

	const TextDocument& m_doc;
	TextStyle&          m_style;
	size_t              m_lineCount;
	VisualBlockList     m_blocks;
};

#endif
