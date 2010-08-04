// VisualLine.h

#ifndef VisualLine_h
#define VisualLine_h

#include "TextRun.h"
#include "ArrayOf.h"
#include "LayoutData.h"
#include <vector>

class VisualPainter;
class VisualSelection;
class TextSelection;

class VisualLine
{
public:
	VisualLine();
	VisualLine( size_t textStart, TextRun* start, TextRun* end );

	void Draw( VisualPainter&, RECT, const LayoutData& ) const;

	VisualSelection MakeVisualSelection( TextSelection, const LayoutData& ) const;

	LONG   CPtoX( size_t, bool trailingEdge, const LayoutData& ) const;
	size_t XtoCP( LONG* x, const LayoutData& ) const;

	size_t TextStart() const { return m_textStart; }
	size_t TextEnd() const   { return m_textEnd; }

	LONG Width() const       { return m_width; }

private:
	LONG RunCPtoX( TextRun* run, size_t cp, bool trailingEdge, const LayoutData& ) const;

	SizedAutoArray<int> VisualToLogicalMapping( const LayoutData& ) const;

	TextRun* RunContaining( size_t pos, int* xStart, const LayoutData& ) const;
	TextRun* RunContaining( int    x,   int* xStart, const LayoutData& ) const;

	LONG m_width;
	size_t m_textStart;
	size_t m_textEnd;
	ArrayOf<TextRun> m_runs;
};

#endif
