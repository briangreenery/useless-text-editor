// TextSquiggle.h

#ifndef TextSquiggle_h
#define TextSquiggle_h

#include <Windows.h>

class TextSquiggle
{
public:
	TextSquiggle();
	~TextSquiggle();

	void Draw( HDC target, int xStart, int xEnd, int y ) const;

private:
	void Resize( int cx ) const;

	HBITMAP m_squiggleSourceBitmap;

	mutable int     m_width;
	mutable HDC     m_squiggleDC;
	mutable HBITMAP m_squiggleRepeatedBitmap;
};

#endif
