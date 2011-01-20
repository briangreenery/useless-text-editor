// UniscribeTextBlock.h

#ifndef UniscribeTextBlock_h
#define UniscribeTextBlock_h

#include "TextBlock.h"
#include "UniscribeLayoutData.h"
#include "TextStyleRun.h"
#include "ArrayOf.h"
#include <Windows.h>
#include <usp10.h>
#include <vector>

class UniscribeAllocator;
class VisualPainter;
class TextSelection;
class TextStyleRegistry;

class UniscribeTextBlock : public TextBlock
{
public:
	UniscribeTextBlock( UniscribeLayoutDataPtr, const TextStyleRegistry& );

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
	void DrawLineBackground( size_t line, VisualPainter&, RECT ) const;
	void DrawLineSelection ( size_t line, VisualPainter&, RECT ) const;
	void DrawLineText      ( size_t line, VisualPainter&, RECT ) const;

	void DrawLineRect( VisualPainter& painter, RECT rect, int xStart, int xEnd, uint32 color ) const;

	ArrayOf<const UniscribeTextRun> LineRuns( size_t line ) const;

	ArrayOf<const TextStyleRun> RunStyles( const UniscribeTextRun&, ArrayOf<const TextStyleRun> ) const;

	size_t TextStart( size_t line ) const;
	size_t TextEnd  ( size_t line ) const;
	int    LineWidth( size_t line ) const;

	int RunCPtoX( const UniscribeTextRun&, size_t cp, bool trailingEdge ) const;
	std::pair<int,int> RunCPtoXRange( const UniscribeTextRun&, size_t start, size_t end ) const;

	int    CPtoX( size_t line, size_t cp, bool trailingEdge ) const;
	size_t XtoCP( size_t line, LONG* x ) const;

	std::vector<int> VisualToLogicalMapping( ArrayOf<const UniscribeTextRun> ) const;

	const UniscribeTextRun* RunContaining( ArrayOf<const UniscribeTextRun>, size_t pos, int* xStart ) const;
	const UniscribeTextRun* RunContaining( ArrayOf<const UniscribeTextRun>, int    x,   int* xStart ) const;

	UniscribeLayoutDataPtr m_data;
	const TextStyleRegistry& m_styleRegistry;
};

#endif
