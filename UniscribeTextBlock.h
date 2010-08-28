// UniscribeTextBlock.h

#ifndef UniscribeTextBlock_h
#define UniscribeTextBlock_h

#include "TextBlock.h"
#include "UniscribeLayoutData.h"
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
	UniscribeTextBlock( UniscribeLayoutDataPtr, TextStyle& );

	virtual void Draw( VisualPainter&, RECT ) const;

	virtual size_t LineCount() const;
	virtual size_t LineContaining( size_t ) const;
	virtual size_t LineStart( int y ) const;
	virtual size_t LineEnd  ( int y ) const;

	virtual POINT PointFromChar( size_t, bool advancing ) const;
	virtual size_t CharFromPoint( POINT* ) const;

	virtual size_t Length() const;
	virtual int Height() const;

	virtual bool EndsWithNewline() const;

private:
	void DrawLineSelection( size_t line, VisualPainter&, RECT ) const;
	void DrawLineText     ( size_t line, VisualPainter&, RECT ) const;

	VisualSelection MakeVisualSelection( size_t line, TextSelection ) const;

	ArrayOf<const UniscribeTextRun> LineRuns( size_t line ) const;

	size_t TextStart( size_t line ) const;
	size_t TextEnd  ( size_t line ) const;
	int    LineWidth( size_t line ) const;

	int RunCPtoX( const UniscribeTextRun*, size_t cp, bool trailingEdge ) const;

	int    CPtoX( ArrayOf<const UniscribeTextRun>, size_t cp, bool trailingEdge ) const;
	size_t XtoCP( ArrayOf<const UniscribeTextRun>, LONG* x ) const;

	std::vector<int> VisualToLogicalMapping( ArrayOf<const UniscribeTextRun> ) const;

	const UniscribeTextRun* RunContaining( ArrayOf<const UniscribeTextRun>, size_t pos, int* xStart ) const;
	const UniscribeTextRun* RunContaining( ArrayOf<const UniscribeTextRun>, int    x,   int* xStart ) const;

	UniscribeLayoutDataPtr m_data;
	TextStyle& m_style;
};

#endif
