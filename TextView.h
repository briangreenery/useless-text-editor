// TextView.h

#ifndef TextView_h
#define TextView_h

#include "Document.h"
#include "CharSelection.h"
#include "TextViewMetrics.h"
#include "TextStyleRegistry.h"
#include "VisualDocument.h"

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
	void OnTimer( UINT_PTR );
	void OnCaptureChanged();
	UINT OnMouseActivate();
	void OnSetCursor();

	void ScrollTo   ( int x, int y );
	void ScrollDelta( int x, int y );
	void OnMouseWheel( int zDelta );
	void OnVScroll( UINT code );
	void OnHScroll( UINT code );
	UINT GetTrackPos( int scrollBar );
	int SnapToLine( int y );

	void OnSetFocus ( HWND );
	void OnKillFocus( HWND );

	void AdvanceCaret( bool wholeWord, bool moveSelection );
	void RetireCaret ( bool wholeWord, bool moveSelection );

	void Home( bool gotoDocStart, bool moveSelection );
	void End ( bool gotoDocEnd,   bool moveSelection );

	void LineUp( bool moveSelection, bool up );

	void Backspace( bool wholeWord );
	void Delete   ( bool wholeWord );
	void Insert   ( ArrayRef<const wchar_t> );

	void SelectAll();
	void SelectWord();

	void Clear( CharSelection );
	void Cut();
	void Copy();
	void Paste();

	void Undo();
	void Redo();

	std::wstring GetText();
	void SetText( ArrayRef<const wchar_t> );

	HWND WindowHandle() const;

private:
	void UpdateLayout( CharChange, CharSelection );
	void UpdateLayout();

	size_t LineNumberDigits();
	bool AdjustGutterWidth();

	void HideCaret();
	void ShowCaret();
	void MoveSelection( CharSelection, bool scrollToCaret );
	void ScrollToCaret();
	void UpdateCaretPos();

	HWND m_hwnd;

	Document          m_doc;
	CharSelection     m_selection;
	TextStyleRegistry m_styleRegistry;
	TextViewMetrics   m_metrics;
	VisualDocument    m_blocks;

	bool     m_isDoingMouseSel;
	UINT_PTR m_mouseSelTimer;
	DWORD    m_lastMouseScrollTick;
	DWORD    m_mouseScrollInterval;

	bool m_isCaretVisible;
	int  m_mouseWheelRemainder;

	POINT m_lineUpStart;
	int   m_lineUpCount;

	enum { lastWasNothing, lastWasInsert, lastWasClear } m_lastEditOperation;
	DWORD m_lastUndoTick;
};

#endif
