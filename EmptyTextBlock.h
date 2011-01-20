// EmptyTextBlock.h

#ifndef EmptyTextBlock_h
#define EmptyTextBlock_h

#include "TextBlock.h"

class TextStyleRegistry;

class EmptyTextBlock : public TextBlock
{
public:
	EmptyTextBlock( bool endsWithNewline, TextStyleRegistry& );

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
	bool m_endsWithNewline;
	TextStyleRegistry& m_styleRegistry;
};

#endif
