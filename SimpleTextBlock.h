// SimpleTextBlock.h

#ifndef SimpleTextBlock_h
#define SimpleTextBlock_h

#include "SimpleLayoutData.h"
#include "TextBlock.h"
#include "ArrayOf.h"
#include <vector>

class TextStyle;

class SimpleTextBlock : public TextBlock
{
public:
	SimpleTextBlock( SimpleLayoutDataPtr, TextStyle& );

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

	size_t TextStart( size_t line ) const;
	size_t TextEnd  ( size_t line ) const;
	int    LineWidth( size_t line ) const;
	int    RunWidth ( const SimpleTextRun* ) const;

	ArrayOf<const SimpleTextRun> LineRuns( size_t line ) const;

	int    CPtoX( size_t line, size_t cp, bool trailingEdge ) const;
	size_t XtoCP( size_t line, LONG* x ) const;

	SimpleLayoutDataPtr m_data;
	TextStyle&          m_style;
};

#endif
