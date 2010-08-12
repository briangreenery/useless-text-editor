// UniscribeTextBlock.h

#ifndef UniscribeTextBlock_h
#define UniscribeTextBlock_h

#include "TextBlock.h"
#include "UniscribeRun.h"
#include "ArrayOf.h"
#include <Windows.h>
#include <usp10.h>
#include <vector>

class UniscribeAllocator;
class VisualPainter;
class VisualSelection;
class TextSelection;
class TextStyle;

class UniscribeTextBlock : public TextBlock
{
public:
	UniscribeTextBlock( UniscribeAllocator&, TextStyle&, bool endsWithNewline );

	virtual void Draw( VisualPainter&, RECT ) const;

	virtual size_t LineCount() const;
	virtual size_t LineContaining( size_t ) const;
	virtual size_t LineStart( int y ) const;
	virtual size_t LineEnd  ( int y ) const;

	virtual POINT PointFromChar( size_t, bool advancing ) const;
	virtual size_t CharFromPoint( POINT* ) const;

	virtual size_t Length() const;
	virtual int Height() const;

	bool EndsWithNewline() const;

private:
	void DrawLineSelection( size_t line, VisualPainter&, RECT ) const;
	void DrawLineText     ( size_t line, VisualPainter&, RECT ) const;

	VisualSelection MakeVisualSelection( size_t line, TextSelection ) const;

	ArrayOf<const UniscribeRun> LineRuns( size_t line ) const;

	size_t TextStart( size_t line ) const;
	size_t TextEnd  ( size_t line ) const;
	int    LineWidth( size_t line ) const;

	int RunCPtoX( const UniscribeRun*, size_t cp, bool trailingEdge ) const;

	int    CPtoX( ArrayOf<const UniscribeRun>, size_t cp, bool trailingEdge ) const;
	size_t XtoCP( ArrayOf<const UniscribeRun>, LONG* x ) const;

	std::vector<int> VisualToLogicalMapping( ArrayOf<const UniscribeRun> ) const;

	const UniscribeRun* RunContaining( ArrayOf<const UniscribeRun>, size_t pos, int* xStart ) const;
	const UniscribeRun* RunContaining( ArrayOf<const UniscribeRun>, int    x,   int* xStart ) const;

	size_t m_length;
	TextStyle& m_style;

	std::vector<size_t>         m_lines;
	std::vector<UniscribeRun>   m_runs;
	std::vector<SCRIPT_ITEM>    m_items;
	std::vector<WORD>           m_logClusters;
	std::vector<WORD>           m_glyphs;
	std::vector<SCRIPT_VISATTR> m_visAttrs;
	std::vector<int>            m_advanceWidths;
	std::vector<GOFFSET>        m_offsets;
};

#endif
