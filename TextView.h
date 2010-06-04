// TextView.h

#ifndef TextView_h
#define TextView_h

#include "TextDocument.h"
#include "TextSelection.h"
#include "TextViewMetrics.h"
#include "TextParagraphs.h"

class TextView
{
public:
	TextView( HWND );

	int  OnCreate( LPCREATESTRUCT );
	void OnSize  ( UINT state, int cx, int cy );
	
	void OnPaint();
	void PaintGutter( HDC hdc, RECT rect );

	void OnChar   ( UINT keyCode, UINT repCnt, UINT flags );
	void OnKeyDown( UINT keyCode, UINT repCnt, UINT flags );

	void OnLButtonDblClk( POINT point );
	void OnLButtonDown  ( POINT point );
	void OnLButtonUp    ( POINT point );
	void OnMouseMove    ( POINT point );
	void OnCaptureChanged();
	UINT OnMouseActivate();
	void OnSetCursor();

	void ScrollTo   ( int x, int y );
	void ScrollDelta( int x, int y );
	void OnMouseWheel( int zDelta );
	void OnVScroll( UINT code );
	void OnHScroll( UINT code );
	UINT GetTrackPos( int scrollBar );

	void OnSetFocus ( HWND );
	void OnKillFocus( HWND );

	void AdvanceCaret( bool wholeWord, bool moveSelection );
	void RetireCaret ( bool wholeWord, bool moveSelection );

	void Home( bool gotoDocStart, bool moveSelection );
	void End ( bool gotoDocEnd,   bool moveSelection );

	void LineUp( bool moveSelection, bool up );

	void Backspace( bool wholeWord );
	void Delete   ( bool wholeWord );
	void Insert   ( UTF16Ref );

	void SelectAll();

	void Clear( bool moveCaret );
	void Cut();
	void Copy();
	void Paste();

	bool CanUndo();
	bool CanRedo();
	void Undo();
	void Redo();

private:
	void UpdateLayout();

	void HideCaret();
	void ShowCaret();
	void MoveCaret( size_t, bool advancing );
	void ScrollToCaret();
	void UpdateCaretPos();

	HWND m_hwnd;

	TextDocument    m_doc;
	TextSelection   m_selection;
	TextViewMetrics m_metrics;
	TextParagraphs  m_paragraphs;

	bool m_isDoingMouseSel;
	bool m_isCaretVisible;
	int  m_mouseWheelRemainder;

	POINT m_lineUpStart;
	int   m_lineUpCount;
};

#endif
