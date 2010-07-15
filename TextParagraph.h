// TextParagraph.h

#ifndef TextParagraph_h
#define TextParagraph_h

#include "LayoutAllocator.h"
#include "LayoutData.h"
#include "VisualLine.h"
#include "TextSelection.h"
#include <vector>

class DocumentReader;

class TextParagraph
{
public:
	TextParagraph();
	TextParagraph( const TextParagraph& ); // This was put here to appease the compiler and probably should die

	SelectionRanges Draw( HDC hdc, RECT, SCRIPT_CACHE, TextSelection, SelectionRanges, bool isLast ) const;

	size_t LineStart( LONG y ) const;
	size_t LineEnd  ( LONG y ) const;

	size_t LineContaining( size_t ) const;
	size_t LineCount() const { return m_lines.size(); }

	POINT PointFromChar( size_t, bool advancing ) const;
	size_t CharFromPoint( POINT* ) const;

	int Width() const     { return m_width; }
	int Height() const    { return LineCount() * LineHeight(); }
	size_t Length() const { return m_length; }
	size_t LengthWithoutNewline() const;

	int LineHeight() const { return 20; }

	bool Valid() const { return m_valid; }
	void Validate( LayoutAllocator& );
	void Validate();
	void ResetLength( size_t );

private:
	bool m_valid;
	LayoutData m_layout;

	typedef std::vector<VisualLine> VisualLines;
	VisualLines m_lines;

	int    m_width;
	size_t m_length;
};

#endif
