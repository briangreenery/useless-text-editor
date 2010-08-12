// SimpleTextBlock.h

#ifndef SimpleTextBlock_h
#define SimpleTextBlock_h

#include "SimpleRun.h"
#include "TextBlock.h"
#include <vector>

class SimpleTextBlock : public TextBlock
{
public:
	SimpleTextBlock( std::vector<size_t>& lines, std::vector<SimpleRun>& runs, std::vector<INT>& positions );

	virtual void Draw( VisualPainter&, RECT ) const;

	virtual size_t LineCount() const;
	virtual size_t LineContaining( size_t ) const;
	virtual size_t LineStart( int y ) const;
	virtual size_t LineEnd  ( int y ) const;

	virtual POINT PointFromChar( size_t, bool advancing );
	virtual size_t CharFromPoint( POINT* ) const;

	virtual size_t Length() const;
	virtual int Height() const;

	virtual bool EndsWithNewline() const;
};

#endif
