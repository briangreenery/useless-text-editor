// VisualBlock.h

#ifndef VisualBlock_h
#define VisualBlock_h

#include "UniscribeLine.h"
#include <vector>
#include <list>

class VisualPainter;
class TextStyle;

class VisualBlock
{
public:
	VisualBlock( size_t length );
	VisualBlock( size_t length, std::vector<UniscribeLine>& );

	void Draw( VisualPainter&, RECT ) const;

	size_t LineCount() const;
	size_t LineContaining( size_t ) const;
	size_t LineStart( int y, TextStyle& ) const;
	size_t LineEnd  ( int y, TextStyle& ) const;

	POINT PointFromChar( size_t, bool advancing, TextStyle& ) const;
	size_t CharFromPoint( POINT*, TextStyle& ) const;

	size_t Length() const;
	int Height( TextStyle& ) const;

	bool EndsWithNewline() const;

private:
	void DrawSelection( const UniscribeLine&, VisualPainter&, RECT ) const;

	size_t m_length;
	std::vector<UniscribeLine> m_lines;
};

typedef std::list<VisualBlock> VisualBlockList;

#endif
