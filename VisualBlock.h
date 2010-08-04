// VisualBlock.h

#ifndef VisualBlock_h
#define VisualBlock_h

#include "LayoutData.h"
#include "VisualLine.h"
#include <vector>
#include <list>

class LayoutAllocator;
class VisualPainter;
class TextStyle;

class VisualBlock
{
public:
	VisualBlock( size_t length );
	VisualBlock( size_t length, LayoutAllocator&, ArrayOf<SCRIPT_ITEM> );

	void Draw( VisualPainter&, RECT ) const;

	size_t LineCount() const;
	size_t LineContaining( size_t ) const;
	size_t LineStart( LONG y, TextStyle& ) const;
	size_t LineEnd  ( LONG y, TextStyle& ) const;

	POINT PointFromChar( size_t, bool advancing, TextStyle& ) const;
	size_t CharFromPoint( POINT*, TextStyle& ) const;

	size_t Length() const;
	LONG Height( TextStyle& ) const;

	bool EndsWithNewline() const;

private:
	void DrawSelection( const VisualLine&, VisualPainter&, RECT ) const;

	size_t m_length;
	LayoutData m_layout;
	std::vector<VisualLine> m_lines;
};

typedef std::list<VisualBlock> VisualBlockList;

#endif
