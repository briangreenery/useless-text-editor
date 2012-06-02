// TextBlock.h

#ifndef TextBlock_h
#define TextBlock_h

#include <Windows.h>
#include <memory>
#include <list>

class TextStyle;
class VisualPainter;

class TextBlock
{
public:
	TextBlock() {}

	virtual void DrawBackground( VisualPainter&, RECT ) const = 0;
	virtual void DrawText      ( VisualPainter&, RECT ) const = 0;

	virtual size_t LineCount() const = 0;
	virtual size_t LineContaining( size_t ) const = 0;
	virtual size_t LineStart( int y ) const = 0;
	virtual size_t LineEnd  ( int y ) const = 0;

	virtual POINT PointFromChar( size_t, bool advancing ) const = 0;
	virtual size_t CharFromPoint( POINT* ) const = 0;

	virtual size_t Length() const = 0;
	virtual int Height() const = 0;

	virtual bool EndsWithNewline() const = 0;

private:
	TextBlock( const TextBlock& );
	TextBlock& operator=( const TextBlock& );
};

typedef std::unique_ptr<TextBlock> TextBlockPtr;
typedef std::list<TextBlockPtr> TextBlockList;

#endif
