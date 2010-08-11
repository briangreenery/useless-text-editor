// TextView.cpp

#include "TextView.h"
#include <Windows.h>
#include <Windowsx.h>
#include <uxtheme.h>

#undef min
#undef max

TextView::TextView( HWND hwnd )
	: m_hwnd( hwnd )
	, m_blocks( m_doc, m_style )
	, m_isDoingMouseSel( false )
	, m_isCaretVisible( false )
	, m_mouseWheelRemainder( 0 )
	, m_lineUpCount( 0 )
{
	m_metrics.gutterWidth = 25;
	m_metrics.marginWidth = 5;
}

int TextView::OnCreate( LPCREATESTRUCT )
{
	m_selection.endPoint.x = 0;
	m_selection.endPoint.y = 0;
	m_selection.start = m_selection.end = m_blocks.CharFromPoint( &m_selection.endPoint );

	return 0;
}

void TextView::OnSize( UINT state, int cx, int cy )
{
	RECT oldClientRect = m_metrics.clientRect;
	GetClientRect( m_hwnd, &m_metrics.clientRect );

	if ( cy != ( oldClientRect.bottom - oldClientRect.top ) )
		m_metrics.linesPerPage = cy / m_style.lineHeight;

	if ( cx != ( oldClientRect.right - oldClientRect.left ) )
		UpdateLayout();

	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( m_hwnd, &ps );

	HPAINTBUFFER hpb = BeginBufferedPaint( ps.hdc, &ps.rcPaint, BPBF_COMPATIBLEBITMAP, NULL, &hdc );

	PaintGutter( hdc, ps.rcPaint );

	RECT fillRect = m_metrics.IntersectWithTextOrMargin( ps.rcPaint, m_hwnd );
	FillRect( hdc, &fillRect, GetSysColorBrush( COLOR_WINDOW ) );

	POINT oldOrigin;
	m_blocks.Draw( m_metrics.ClientToText( hdc, &oldOrigin ),
	               m_metrics.ClientToText( m_metrics.IntersectWithText( ps.rcPaint, m_hwnd ) ),
	               m_selection );

	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y, NULL );

	EndBufferedPaint( hpb, TRUE );
	EndPaint( m_hwnd, &ps );
}

void TextView::PaintGutter( HDC hdc, RECT rect )
{
	RECT gutterRect = m_metrics.IntersectWithGutter( rect, m_hwnd );
	FillRect( hdc, &gutterRect, m_style.gutterBrush );
}

void TextView::OnChar( UINT keyCode, UINT repCnt, UINT flags )
{
	bool isCtrlPressed = GetKeyState( VK_CONTROL ) < 0;

	if ( isCtrlPressed || keyCode == VK_BACK )
		return;

	while ( repCnt-- > 0 )
	{
		UTF16::Unit unit = ( keyCode == VK_RETURN ) ? 0x0A : keyCode;
		Insert( UTF16Ref( &unit, 1 ) );
	}
}

void TextView::OnKeyDown( UINT keyCode, UINT repCnt, UINT flags )
{
	bool isCtrlPressed  = GetKeyState( VK_CONTROL ) < 0;
	bool isShiftPressed = GetKeyState( VK_SHIFT   ) < 0;

	if ( keyCode != VK_UP && keyCode != VK_DOWN )
		m_lineUpCount = 0;

	while ( repCnt-- > 0 )
	{
		switch ( keyCode )
		{
		case VK_LEFT:  RetireCaret ( isCtrlPressed, isShiftPressed ); break;
		case VK_RIGHT: AdvanceCaret( isCtrlPressed, isShiftPressed ); break;

		case VK_HOME: Home( isCtrlPressed, isShiftPressed ); break;
		case VK_END:  End ( isCtrlPressed, isShiftPressed ); break;

		case VK_UP:   LineUp( isShiftPressed, true  ); break;
		case VK_DOWN: LineUp( isShiftPressed, false ); break;

		case VK_BACK:   Backspace( isCtrlPressed ); break;
		case VK_DELETE: Delete   ( isCtrlPressed ); break;

		case 'A': if ( isCtrlPressed ) SelectAll();  break;
		case 'W': if ( isCtrlPressed ) SelectWord(); break;

		case 'X': if ( isCtrlPressed ) Cut();   break;
		case 'C': if ( isCtrlPressed ) Copy();  break;
		case 'V': if ( isCtrlPressed ) Paste(); break;

		case 'Z': if ( isCtrlPressed ) Undo(); break;
		case 'Y': if ( isCtrlPressed ) Redo(); break;
		}
	}
}

void TextView::OnLButtonDown( POINT point )
{
	m_lineUpCount = 0;

	if ( !m_metrics.IsInTextOrMargin( point, m_hwnd ) )
		return;

	m_selection.endPoint = m_metrics.ClientToText( point );
	m_selection.start = m_selection.end = m_blocks.CharFromPoint( &m_selection.endPoint );

	ScrollToCaret();
	UpdateCaretPos();
	InvalidateRect( m_hwnd, NULL, FALSE );

	SetCapture( m_hwnd );
	m_isDoingMouseSel = true;
}

void TextView::OnLButtonDblClk( POINT point )
{
	SelectWord();
}

void TextView::OnLButtonUp( POINT point )
{
	ReleaseCapture();
}

void TextView::OnMouseMove( POINT point )
{
	if ( !m_isDoingMouseSel )
		return;

	// "ClampToVisibleText" firefox suggests that maybe the yOffset should be in terms of pixels, and that we should snap whenever the mouse is finished.
	m_selection.endPoint = m_metrics.ClientToText( point );
	m_selection.end = m_blocks.CharFromPoint( &m_selection.endPoint );
	UpdateCaretPos();
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::OnCaptureChanged()
{
	m_isDoingMouseSel = false;
}

UINT TextView::OnMouseActivate()
{
	SetFocus( m_hwnd );
	return MA_ACTIVATE;
}

void TextView::OnSetCursor()
{
	DWORD pos = GetMessagePos();
	POINT point = { GET_X_LPARAM( pos ), GET_Y_LPARAM( pos ) };
	ScreenToClient( m_hwnd, &point );

	if ( m_isDoingMouseSel || m_metrics.IsInTextOrMargin( point, m_hwnd ) )
		SetCursor( LoadCursor( NULL, IDC_IBEAM ) );
	else
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void TextView::OnSetFocus( HWND )
{
	CreateCaret( m_hwnd, NULL, m_metrics.caretWidth, m_style.lineHeight );
	::ShowCaret( m_hwnd );
	UpdateCaretPos();
}

void TextView::OnKillFocus( HWND )
{
	m_isCaretVisible = false;
	DestroyCaret();
}

void TextView::AdvanceCaret( bool wholeWord, bool moveSelection )
{
	if ( !wholeWord && !moveSelection && !m_selection.IsEmpty() )
	{
		m_selection.start = m_selection.end = m_selection.Max();
	}
	else
	{
		m_selection.end = wholeWord
		                     ? m_doc.NextNonWhitespace( m_doc.NextWordStop( m_selection.end ) )
		                     : m_doc.NextCharStop( m_selection.end );

		if ( !moveSelection )
			m_selection.start = m_selection.end;
	}

	MoveCaret( m_selection.end, true );
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::RetireCaret( bool wholeWord, bool moveSelection )
{
	if ( !wholeWord && !moveSelection && !m_selection.IsEmpty() )
	{
		m_selection.start = m_selection.end = m_selection.Min();
	}
	else
	{
		m_selection.end = wholeWord
		                     ? m_doc.PrevWordStop( m_doc.PrevNonWhitespace( m_selection.end ) )
		                     : m_doc.PrevCharStop( m_selection.end );

		if ( !moveSelection )
			m_selection.start = m_selection.end;
	}

	MoveCaret( m_selection.end, false );
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::Home( bool gotoDocStart, bool moveSelection )
{
	m_selection.end = gotoDocStart
	                     ? 0
	                     : m_blocks.LineStart( m_selection.endPoint.y );

	if ( !moveSelection )
		m_selection.start = m_selection.end;

	MoveCaret( m_selection.end, false );
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::End( bool gotoDocEnd, bool moveSelection )
{
	m_selection.end = gotoDocEnd
	                     ? m_doc.Length()
	                     : m_blocks.LineEnd( m_selection.endPoint.y );

	if ( !moveSelection )
		m_selection.start = m_selection.end;

	MoveCaret( m_selection.end, true );
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::LineUp( bool moveSelection, bool up )
{
	if ( m_lineUpCount == 0 )
		m_lineUpStart = m_selection.endPoint;

	int count = up
	            ? m_lineUpCount + 1
	            : m_lineUpCount - 1;

	int y = m_lineUpStart.y - ( m_style.lineHeight * count );

	if ( 0 <= y && y < m_blocks.Height() )
	{
		m_lineUpCount = count;
		m_selection.endPoint.y = y;
		m_selection.endPoint.x = m_lineUpStart.x;
		m_selection.end = m_blocks.CharFromPoint( &m_selection.endPoint );
	}

	if ( !moveSelection )
		m_selection.start = m_selection.end;

	ScrollToCaret();
	UpdateCaretPos();
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::Backspace( bool wholeWord )
{
	if ( m_selection.IsEmpty() )
	{
		if ( m_selection.end == 0 )
			return;

		m_selection.end = wholeWord
		                     ? m_doc.PrevWordStop( m_selection.end - 1 )
		                     : m_selection.end - 1;
	}

	Clear( true );
}

void TextView::Delete( bool wholeWord )
{
	if ( m_selection.IsEmpty() )
	{
		if ( m_selection.end == m_doc.Length() )
			return;

		m_selection.end = wholeWord
		                     ? m_doc.NextWordStop( m_selection.end + 1 )
		                     : m_doc.NextCharStop( m_selection.end + 1 );
	}

	Clear( true );
}

void TextView::Insert( UTF16Ref text )
{
	Clear( false );
	Assert( m_selection.IsEmpty() );

	TextChange change = m_doc.Insert( m_selection.end, text );
	UpdateLayout( change );

	m_selection.start = m_selection.end = m_selection.end + change.count;
	MoveCaret( m_selection.end, true );
}

void TextView::SelectAll()
{
	m_selection.start = 0;
	m_selection.end   = m_doc.Length();

	MoveCaret( m_selection.end, true );
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::SelectWord()
{
	std::pair<size_t, size_t> word = m_doc.WordAt( m_selection.end );

	m_selection.start = word.first;
	m_selection.end   = word.second;

	MoveCaret( m_selection.end, true );
	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::Clear( bool moveCaret )
{
	if ( m_selection.IsEmpty() )
		return;

	TextChange change = m_doc.Delete( m_selection.Min(), m_selection.Size() );
	UpdateLayout( change );

	m_selection.start = m_selection.end = m_selection.Min();

	if ( moveCaret )
		MoveCaret( m_selection.end, true );
}

void TextView::Cut()
{
	Copy();
	Clear( true );
}

void TextView::Copy()
{
	if ( m_selection.IsEmpty() )
		return;
	
	if ( !OpenClipboard( NULL ) )
		return;

	EmptyClipboard();

	size_t sizeWithCRLF = m_doc.SizeWithCRLF( m_selection.Min(), m_selection.Size() );
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE, ( sizeWithCRLF + 1 ) * sizeof( UTF16::Unit ) );

	if ( hGlobal != 0 )
	{
		UTF16::Unit* dest = static_cast<UTF16::Unit*>( GlobalLock( hGlobal ) );

		if ( dest != 0 )
		{
			m_doc.ReadWithCRLF( m_selection.Min(), m_selection.Size(), ArrayOf<UTF16::Unit>( dest, dest + sizeWithCRLF ) );
			dest[sizeWithCRLF] = UTF16::Unit( 0 );

			GlobalUnlock( hGlobal );

			if ( !SetClipboardData( CF_UNICODETEXT, hGlobal ) )
				GlobalFree( hGlobal );
		}
	}

	CloseClipboard();
}

void TextView::Paste()
{
	if ( !IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		return;

	if ( !OpenClipboard( m_hwnd ) )
		return;

	HGLOBAL hClipboardData = GetClipboardData( CF_UNICODETEXT );

	if ( hClipboardData != 0 )
	{
		UTF16::Unit* clipboardString = static_cast<UTF16::Unit*>( GlobalLock( hClipboardData ) );

		if ( clipboardString != 0 )
			Insert( UTF16Ref( clipboardString, wcslen( clipboardString ) ) );

		GlobalUnlock( hClipboardData );
	}

	CloseClipboard();
}

bool TextView::CanUndo()
{
	return false;
}

bool TextView::CanRedo()
{
	return false;
}

void TextView::Undo()
{
	if ( !CanUndo() )
		return;
}

void TextView::Redo()
{
	if ( !CanRedo() )
		return;
}

void TextView::UpdateLayout( TextChange change )
{
	HDC hdc = GetDC( m_hwnd );

	RECT rect = m_metrics.TextRect( m_hwnd );
	m_blocks.Update( hdc, rect.right - rect.left, change );

	ReleaseDC( m_hwnd, hdc );

	InvalidateRect( m_hwnd, NULL, FALSE );
	ScrollDelta( 0, 0 );
}

void TextView::UpdateLayout()
{
	HDC hdc = GetDC( m_hwnd );

	RECT rect = m_metrics.TextRect( m_hwnd );
	m_blocks.UpdateAll( hdc, rect.right - rect.left );

	ReleaseDC( m_hwnd, hdc );

	InvalidateRect( m_hwnd, NULL, FALSE );
	ScrollDelta( 0, 0 );
}

void TextView::MoveCaret( size_t pos, bool advancing )
{
	m_selection.endPoint = m_blocks.PointFromChar( pos, advancing );
	ScrollToCaret();
	UpdateCaretPos();
}

void TextView::ScrollToCaret()
{
	RECT textRect = m_metrics.ClientToText( m_metrics.TextRect( m_hwnd ) );

	int textHeight         = textRect.bottom - textRect.top;
	int adjustedTextHeight = textHeight - textHeight % m_style.lineHeight;

	int xOffset = m_metrics.xOffset;
	int yOffset = m_metrics.yOffset;

	if ( m_selection.endPoint.y + m_style.lineHeight > textRect.bottom )
		yOffset = m_selection.endPoint.y + m_style.lineHeight - adjustedTextHeight;
	else if ( m_selection.endPoint.y < textRect.top )
		yOffset = m_selection.endPoint.y;

	if ( xOffset != m_metrics.xOffset || yOffset != m_metrics.yOffset )
		ScrollTo( xOffset, yOffset );
}

void TextView::UpdateCaretPos()
{
	POINT point = m_metrics.TextToClient( m_selection.endPoint );

	RECT rect = { point.x, point.y, point.x + m_metrics.caretWidth, point.y + m_style.lineHeight };
	RECT intersection = m_metrics.IntersectWithText( rect, m_hwnd );

	if ( IsRectEmpty( &intersection ) )
	{
		HideCaret();
	}
	else
	{
		// If the position on screen doesn't change, then the caret keeps blinking away.
		// We don't want that when scrolling up or down.
		HideCaret();
		SetCaretPos( 0, 0 );
		SetCaretPos( point.x, point.y );
		ShowCaret();
	}
}

void TextView::HideCaret()
{
	if ( m_isCaretVisible )
	{
		m_isCaretVisible = false;
		::HideCaret( m_hwnd );
	}
}

void TextView::ShowCaret()
{
	if ( !m_isCaretVisible )
	{
		m_isCaretVisible = true;
		::ShowCaret( m_hwnd );
	}
}

void TextView::ScrollTo( int x, int y )
{
	y = std::min( y, int( ( m_blocks.LineCount() - m_metrics.linesPerPage ) * m_style.lineHeight ) );
	y = std::max( y, int( 0 ) );

	//ScrollWindowEx( m_hwnd, 0, ( m_metrics.yOffset - y ) * m_style.lineHeight, NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE );
	InvalidateRect( m_hwnd, NULL, FALSE );

	m_metrics.yOffset = y;

	SCROLLINFO si = { sizeof si };
	si.fMask      = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	si.nPage      = m_metrics.linesPerPage * m_style.lineHeight;
	si.nMin       = 0;
	si.nMax       = ( m_blocks.LineCount() - 1 ) * m_style.lineHeight;
	si.nPos       = m_metrics.yOffset;

	SetScrollInfo( m_hwnd, SB_VERT, &si, TRUE );
	UpdateCaretPos();
}

void TextView::ScrollDelta( int x, int y )
{
	ScrollTo( m_metrics.xOffset + x, m_metrics.yOffset + y );
}

void TextView::OnMouseWheel( int zDelta )
{
	UINT scrollLines;
	if ( !SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0 ) )
		scrollLines = 3;

	if ( scrollLines == WHEEL_PAGESCROLL )
		scrollLines = m_metrics.linesPerPage;

	if ( scrollLines == 0 )
		return;

	zDelta += m_mouseWheelRemainder;

	int dy = zDelta * (int)scrollLines / WHEEL_DELTA;
	m_mouseWheelRemainder = zDelta - dy * WHEEL_DELTA / (int)scrollLines;

	ScrollDelta( 0, -dy * m_style.lineHeight );
}

void TextView::OnVScroll( UINT code )
{
	switch ( code )
	{
	case SB_LINEUP:         ScrollDelta( 0, -m_style.lineHeight ); break;
	case SB_LINEDOWN:       ScrollDelta( 0,  m_style.lineHeight ); break;
	case SB_PAGEUP:         ScrollDelta( 0, -m_metrics.linesPerPage * m_style.lineHeight ); break;
	case SB_PAGEDOWN:       ScrollDelta( 0,  m_metrics.linesPerPage * m_style.lineHeight ); break;
	case SB_THUMBPOSITION:  ScrollTo( m_metrics.xOffset, SnapToLine( GetTrackPos( SB_VERT ) ) ); break;
	case SB_THUMBTRACK:     ScrollTo( m_metrics.xOffset, SnapToLine( GetTrackPos( SB_VERT ) ) ); break;
	case SB_TOP:            ScrollTo( m_metrics.xOffset, 0 );                      break;
	case SB_BOTTOM:         ScrollTo( m_metrics.xOffset, MAXLONG );                break;
	}
}

void TextView::OnHScroll( UINT code )
{
}

UINT TextView::GetTrackPos( int scrollBar )
{
	SCROLLINFO si = { sizeof si, SIF_TRACKPOS };

	if ( GetScrollInfo( m_hwnd, scrollBar, &si ) )
		return si.nTrackPos;

	return 0;
}

int TextView::SnapToLine( int y )
{
	return m_style.lineHeight * ( y / m_style.lineHeight );
}
