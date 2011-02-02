// TextSquiggle.cpp

#include "TextSquiggle.h"

// These are the raw RGB values for the squiggle, with the alpha pre-multiplied.

static DWORD squiggle[18] =
{
	0xfbfb5555, 0xe2e24d4d, 0x0d0d0404, 0x01010000, 0x35351212, 0xf4f45353,
	0x21210b0b, 0xbdbd4040, 0xd7d74949, 0x3f3f1515, 0xe0e04c4c, 0xadad3b3b,
	0x00000000, 0x24240c0c, 0xc8c84444, 0xffff5757, 0xb0b03c3c, 0x03030101,
};

static int squiggleWidth  = 6;
static int squiggleHeight = 3;

TextSquiggle::TextSquiggle()
	: m_squiggleDC( 0 )
	, m_squiggleSourceBitmap( 0 )
	, m_squiggleRepeatedBitmap( 0 )
	, m_width( 0 )
{
	HDC windowDC = GetDC( NULL );

	m_squiggleDC           = CreateCompatibleDC( windowDC );
	m_squiggleSourceBitmap = CreateCompatibleBitmap( windowDC, squiggleWidth, squiggleHeight );

	BITMAPINFO info              = {};
	info.bmiHeader.biSize        = sizeof( info.bmiHeader );
	info.bmiHeader.biWidth       = squiggleWidth;
	info.bmiHeader.biHeight      = squiggleHeight;
	info.bmiHeader.biPlanes      = 1;
	info.bmiHeader.biBitCount    = 32;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage   = sizeof( squiggle );

	SetDIBits( m_squiggleDC, m_squiggleSourceBitmap, 0, squiggleHeight, squiggle, &info, DIB_RGB_COLORS );

	ReleaseDC( NULL, windowDC );
}

TextSquiggle::~TextSquiggle()
{
	if ( m_squiggleDC )
		DeleteDC( m_squiggleDC );
	if ( m_squiggleSourceBitmap )
		DeleteObject( m_squiggleSourceBitmap );
	if ( m_squiggleRepeatedBitmap )
		DeleteObject( m_squiggleRepeatedBitmap );
}

void TextSquiggle::Resize( int width ) const
{
	if ( m_width >= width )
		return;

	m_width = width;

	HDC windowDC = GetDC( NULL );
	HDC sourceDC = CreateCompatibleDC( windowDC );

	if ( m_squiggleRepeatedBitmap )
		DeleteObject( m_squiggleRepeatedBitmap );
	m_squiggleRepeatedBitmap = CreateCompatibleBitmap( windowDC, width, squiggleHeight );

	SelectObject( sourceDC,     m_squiggleSourceBitmap   );
	SelectObject( m_squiggleDC, m_squiggleRepeatedBitmap );

	for ( int i = 0; i < width; i += squiggleWidth )
		BitBlt( m_squiggleDC, i, 0, squiggleWidth, squiggleHeight, sourceDC, 0, 0, SRCCOPY );

	DeleteDC( sourceDC );
	ReleaseDC( NULL, windowDC );
}

void TextSquiggle::Draw( HDC target, int xStart, int xEnd, int y ) const
{
	Resize( xEnd - xStart );

	if ( m_squiggleDC == 0 || m_squiggleRepeatedBitmap == 0 )
		return;

	BLENDFUNCTION blendFunction       = {};
	blendFunction.SourceConstantAlpha = 255;
	blendFunction.AlphaFormat         = AC_SRC_ALPHA;

	AlphaBlend( target, xStart, y - squiggleHeight, xEnd - xStart, squiggleHeight, m_squiggleDC, 0, 0, xEnd - xStart, 3, blendFunction );
}
