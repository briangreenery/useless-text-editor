// VisualLine.h

#ifndef VisualLine_h
#define VisualLine_h

#include "TextRun.h"
#include "ArrayOf.h"
#include "LayoutData.h"
#include <vector>

typedef std::pair<int, int> SelectionRange;
typedef std::vector<SelectionRange> SelectionRanges;

void AddSelectionRange( SelectionRange, SelectionRanges& );

class VisualLine
{
public:
	VisualLine();
	VisualLine( size_t textStart, TextRun* start, TextRun* end );

	void Draw( HDC hdc, RECT rect, const LayoutData&, SCRIPT_CACHE ) const;

	SelectionRanges MakeSelectionRanges( size_t selStart, size_t selEnd, const LayoutData& ) const;

	LONG   CPtoX( size_t, bool trailingEdge, const LayoutData& ) const;
	size_t XtoCP( LONG* x, const LayoutData& ) const;

	size_t TextStart() const { return m_textStart; }
	size_t TextEnd() const   { return m_textEnd; }

	int Width() const        { return m_width; }

private:
	LONG RunCPtoX( TextRun* run, size_t cp, bool trailingEdge, const LayoutData& layout ) const;

	SizedAutoArray<int> VisualToLogicalMapping( const LayoutData& ) const;

	TextRun* RunContaining( size_t pos, int* xStart, const LayoutData& ) const;
	TextRun* RunContaining( int    x,   int* xStart, const LayoutData& ) const;

	int m_width;
	size_t m_textStart;
	size_t m_textEnd;
	ArrayOf<TextRun> m_runs;
};

#endif
