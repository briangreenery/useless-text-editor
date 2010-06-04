// TextViewMetrics.h

#ifndef TextViewMetrics_h
#define TextViewMetrics_h

#include <Windows.h>

class TextViewMetrics
{
public:
	TextViewMetrics();

	int CaretWidth() const                   { return m_caretWidth; }

	int MarginWidth() const                  { return m_marginWidth; }
	int GutterWidth() const                  { return m_gutterWidth; }

	int LineHeight() const                   { return m_lineHeight; }
	int LinesPerPage() const                 { return m_linesPerPage; }

	int XOffset() const                      { return m_xOffset; }
	int YOffset() const                      { return m_yOffset; }

	void SetCaretWidth( int caretWidth )     { m_caretWidth = caretWidth; }

	void SetMarginWidth( int marginWidth )   { m_marginWidth = marginWidth; }
	void SetGutterWidth( int gutterWidth )   { m_gutterWidth = gutterWidth; }

	void SetLineHeight( int lineHeight )     { m_lineHeight = lineHeight; }
	void SetLinesPerPage( int linesPerPage ) { m_linesPerPage = linesPerPage; }

	void SetXOffset( int xOffset )           { m_xOffset = xOffset; }
	void SetYOffset( int yOffset )           { m_yOffset = yOffset; }

	RECT GutterRect      ( HWND ) const;
	RECT TextRect        ( HWND ) const;
	RECT TextOrMarginRect( HWND ) const;

	RECT IntersectWithGutter      ( RECT, HWND ) const;
	RECT IntersectWithText        ( RECT, HWND ) const;
	RECT IntersectWithTextOrMargin( RECT, HWND ) const;

	bool IsInGutter      ( POINT, HWND ) const;
	bool IsInText        ( POINT, HWND ) const;
	bool IsInTextOrMargin( POINT, HWND ) const;

	HDC   ClientToGutter( HDC ) const;
	RECT  ClientToGutter( RECT ) const;
	POINT ClientToGutter( POINT ) const;
	POINT GutterToClient( POINT ) const;

	HDC   ClientToText( HDC ) const;
	RECT  ClientToText( RECT ) const;
	POINT ClientToText( POINT ) const;
	POINT TextToClient( POINT ) const;

private:
	int m_caretWidth;

	int m_marginWidth;
	int m_gutterWidth;

	int m_lineHeight;
	int m_linesPerPage;

	int m_xOffset;
	int m_yOffset;
};

#endif
