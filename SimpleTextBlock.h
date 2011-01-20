// SimpleTextBlock.h

#ifndef SimpleTextBlock_h
#define SimpleTextBlock_h

#include "SimpleLayoutData.h"
#include "TextStyleRun.h"
#include "TextBlock.h"
#include "ArrayOf.h"
#include "Integers.h"
#include <vector>

class TextStyleRegistry;

class SimpleTextBlock : public TextBlock
{
public:
	SimpleTextBlock( SimpleLayoutDataPtr, const TextStyleRegistry& );

	virtual void DrawBackground( VisualPainter&, RECT ) const;
	virtual void DrawText      ( VisualPainter&, RECT ) const;

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

	void DrawLineRect( VisualPainter&, RECT, int start, int end, uint32 color ) const;

	size_t TextStart( size_t line ) const;
	size_t TextEnd  ( size_t line ) const;
	int    LineWidth( size_t line ) const;
	int    RunWidth ( const SimpleTextRun* ) const;

	ArrayOf<const SimpleTextRun> LineRuns( size_t line ) const;

	int    CPtoX( size_t line, size_t cp, bool trailingEdge ) const;
	size_t XtoCP( size_t line, LONG* x ) const;

	SimpleLayoutDataPtr m_data;
	const TextStyleRegistry&  m_styleRegistry;
};

#endif
