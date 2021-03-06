// TextView.cpp

#include "TextView.h"
#include <Windows.h>
#include <Windowsx.h>
#include <uxtheme.h>

#include "TextMateAnnotator.h"

TextView::TextView( HWND hwnd )
	: m_hwnd( hwnd )
	, m_blocks( m_doc, m_styleRegistry )
	, m_isDoingMouseSel( false )
	, m_mouseSelTimer( 0 )
	, m_mouseScrollInterval( 50 )
	, m_lastMouseScrollTick( 0 )
	, m_isCaretVisible( false )
	, m_mouseWheelRemainder( 0 )
	, m_lineUpCount( 0 )
	, m_lastUndoTick( 0 )
	, m_lastEditOperation( lastWasNothing )
{
	m_metrics.gutterWidth = m_styleRegistry.AvgCharWidth() * 5;
	m_metrics.marginWidth = m_styleRegistry.AvgCharWidth();

	bool sublimejs = true;

	if ( sublimejs )
	{
		TextMateAnnotator* annotator = new TextMateAnnotator( m_doc, m_styleRegistry );
		annotator->SetLanguageFile( "C:\\Users\\Brian\\Desktop\\Projects\\TextEditor\\JavaScript.tmLanguage" );
		m_styleRegistry.SetAnnotator( annotator );

		TextTheme theme;
		theme.gutterBkColor   = RGB( 39, 40, 34 );
		theme.gutterLineColor = RGB( 39, 40, 34 );
		theme.selectionColor  = RGB( 73, 72, 62 );

		uint32_t normal  = theme.AddFont( L"Consolas", false, false );
		uint32_t italics = theme.AddFont( L"Consolas", false, true );

		theme.SetStyle( "default",              TextStyle( normal,  RGB( 255, 255, 255 ), RGB( 39, 40, 34 ) ) );
		theme.SetStyle( "storage",              TextStyle( italics, RGB( 102, 217, 239 ), TextStyle::useDefault ) );
		theme.SetStyle( "support",              TextStyle( italics, RGB( 102, 217, 239 ), TextStyle::useDefault ) );
		theme.SetStyle( "support.function",     TextStyle( normal,  RGB( 102, 217, 239 ), TextStyle::useDefault ) );
		theme.SetStyle( "entity.name.function", TextStyle( normal,  RGB( 166, 226,  46 ), TextStyle::useDefault ) );
		theme.SetStyle( "keyword",              TextStyle( normal,  RGB( 249,  38, 114 ), TextStyle::useDefault ) );
		theme.SetStyle( "constant",             TextStyle( normal,  RGB( 174, 129, 255 ), TextStyle::useDefault ) );
		theme.SetStyle( "variable",             TextStyle( italics, RGB( 253, 151,  32 ), TextStyle::useDefault ) );
		theme.SetStyle( "comment",              TextStyle( normal,  RGB( 117, 113,  94 ), TextStyle::useDefault ) );
		theme.SetStyle( "string",               TextStyle( normal,  RGB( 230, 219, 116 ), TextStyle::useDefault ) );

		m_styleRegistry.SetTheme( theme );
	}
}

int TextView::OnCreate( LPCREATESTRUCT )
{
	POINT point = {};
	m_selection.start = m_selection.end = m_blocks.CharFromPoint( &point );
	return 0;
}

void TextView::OnSize( UINT state, int cx, int cy )
{
	RECT oldClientRect = m_metrics.clientRect;
	GetClientRect( m_hwnd, &m_metrics.clientRect );

	if ( cy != ( oldClientRect.bottom - oldClientRect.top ) )
		m_metrics.linesPerPage = cy / m_styleRegistry.LineHeight();

	if ( cx != ( oldClientRect.right - oldClientRect.left ) )
		UpdateLayout();

	ScrollDelta( 0, 0 );
}

void TextView::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( m_hwnd, &ps );

	HPAINTBUFFER hpb = BeginBufferedPaint( ps.hdc, &ps.rcPaint, BPBF_COMPATIBLEBITMAP, NULL, &hdc );

	PaintGutter( hdc, ps.rcPaint );

	POINT oldOrigin;
	m_blocks.DrawLineNumbers( m_metrics.ClientToGutter( hdc, &oldOrigin ),
	                          m_metrics.ClientToGutter( m_metrics.IntersectWithGutter( ps.rcPaint, m_hwnd ) ) );
	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y, NULL );

	RECT fillRect = m_metrics.IntersectWithTextOrMargin( ps.rcPaint, m_hwnd );
	SetBkColor( hdc, m_styleRegistry.DefaultStyle().bkColor );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &fillRect, L"", 0, NULL );

	m_blocks.DrawText( m_metrics.ClientToText( hdc, &oldOrigin ),
	                   m_metrics.ClientToText( m_metrics.IntersectWithText( ps.rcPaint, m_hwnd ) ),
	                   m_selection );
	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y, NULL );

	EndBufferedPaint( hpb, TRUE );
	EndPaint( m_hwnd, &ps );
}

void TextView::PaintGutter( HDC hdc, RECT rect )
{
	RECT gutterRect = m_metrics.IntersectWithGutter( rect, m_hwnd );

	COLORREF oldBkColor = SetBkColor( hdc, m_styleRegistry.Theme().gutterBkColor );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE|ETO_CLIPPED, &gutterRect, L"", 0, NULL );

	gutterRect.left = gutterRect.right - 1;
	SetBkColor( hdc, m_styleRegistry.Theme().gutterLineColor );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE|ETO_CLIPPED, &gutterRect, L"", 0, NULL );

	SetBkColor( hdc, oldBkColor );
}

void TextView::OnChar( UINT keyCode, UINT repCnt, UINT flags )
{
	bool isCtrlPressed = GetKeyState( VK_CONTROL ) < 0;

	if ( isCtrlPressed || keyCode == VK_BACK )
		return;

	for ( ; repCnt > 0; --repCnt )
	{
		wchar_t unit = ( keyCode == VK_RETURN ) ? 0x0A : keyCode;
		Insert( ArrayRef<const wchar_t>( &unit, 1 ) );
	}
}

void TextView::OnKeyDown( UINT keyCode, UINT repCnt, UINT flags )
{
	bool isCtrlPressed  = GetKeyState( VK_CONTROL ) < 0;
	bool isShiftPressed = GetKeyState( VK_SHIFT   ) < 0;

	if ( keyCode != VK_UP && keyCode != VK_DOWN )
		m_lineUpCount = 0;

	for ( ; repCnt > 0; --repCnt )
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

	m_caretPoint = m_metrics.ClientToText( point );
	CharSelection selection( m_selection.start, m_blocks.CharFromPoint( &m_caretPoint ) );

	bool isShiftPressed = GetKeyState( VK_SHIFT ) < 0;
	if ( !isShiftPressed )
		selection.start = selection.end;

	MoveSelection( selection, true );

	SetCapture( m_hwnd );
	m_mouseSelTimer = SetTimer( m_hwnd, 1, m_mouseScrollInterval, NULL );
	m_lastMouseScrollTick = GetTickCount();
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

	DWORD currentTick = GetTickCount();
	if ( currentTick - m_lastMouseScrollTick >= m_mouseScrollInterval )
	{
		if ( point.y < m_metrics.clientRect.top )
			ScrollDelta( 0, 3*-m_styleRegistry.LineHeight() );
		else if ( point.y >= m_metrics.clientRect.bottom )
			ScrollDelta( 0, 3*m_styleRegistry.LineHeight() );

		m_lastMouseScrollTick = currentTick;
	}

	point.y = (std::min)( m_metrics.clientRect.bottom, point.y );
	point.y = (std::max)( m_metrics.clientRect.top,    point.y );

	m_caretPoint = m_metrics.ClientToText( point );
	CharSelection selection( m_selection.start, m_blocks.CharFromPoint( &m_caretPoint ) );
	MoveSelection( selection, false );
}

void TextView::OnTimer( UINT_PTR id )
{
	if ( id != m_mouseSelTimer )
		return;

	assert( m_isDoingMouseSel );

	POINT point;
	GetCursorPos( &point );
	ScreenToClient( m_hwnd, &point );

	OnMouseMove( point );
}

void TextView::OnCaptureChanged()
{
	KillTimer( m_hwnd, m_mouseSelTimer );
	m_isDoingMouseSel = false;
}

UINT TextView::OnMouseActivate()
{
	SetFocus( m_hwnd );
	return MA_ACTIVATE;
}

void TextView::OnSetCursor()
{
	POINT point;
	GetCursorPos( &point );
	ScreenToClient( m_hwnd, &point );

	if ( m_isDoingMouseSel || m_metrics.IsInTextOrMargin( point, m_hwnd ) )
		SetCursor( LoadCursor( NULL, IDC_IBEAM ) );
	else
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
}

void TextView::OnSetFocus( HWND )
{
	CreateCaret( m_hwnd, NULL, m_metrics.caretWidth, m_styleRegistry.LineHeight() );
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
	CharSelection selection = m_selection;

	if ( !wholeWord && !moveSelection && !selection.IsEmpty() )
	{
		selection.start = selection.end = selection.Max();
	}
	else
	{
		selection.end = wholeWord
		                   ? m_doc.Chars().NextNonWhitespace( m_doc.Chars().NextWordStop( selection.end ) )
		                   : m_doc.Chars().NextCharStop( selection.end );

		if ( !moveSelection )
			selection.start = selection.end;
	}

	MoveSelection( selection, true );
}

void TextView::RetireCaret( bool wholeWord, bool moveSelection )
{
	CharSelection selection = m_selection;

	if ( !wholeWord && !moveSelection && !selection.IsEmpty() )
	{
		selection.start = selection.end = selection.Min();
	}
	else
	{
		selection.end = wholeWord
		                   ? m_doc.Chars().PrevWordStop( m_doc.Chars().PrevNonWhitespace( selection.end ) )
		                   : m_doc.Chars().PrevCharStop( selection.end );

		if ( !moveSelection )
			selection.start = selection.end;
	}

	MoveSelection( selection, true, directionRetiring );
}

void TextView::Home( bool gotoDocStart, bool moveSelection )
{
	CharSelection selection = m_selection;

	selection.end = gotoDocStart
	                   ? 0
	                   : m_blocks.LineStart( m_caretPoint.y );

	if ( !moveSelection )
		selection.start = selection.end;

	MoveSelection( selection, true, directionRetiring );
}

void TextView::End( bool gotoDocEnd, bool moveSelection )
{
	CharSelection selection = m_selection;

	selection.end = gotoDocEnd
	                   ? m_doc.Length()
	                   : m_blocks.LineEnd( m_caretPoint.y );

	if ( !moveSelection )
		selection.start = selection.end;

	MoveSelection( selection, true, directionAdvancing );
}

void TextView::LineUp( bool moveSelection, bool up )
{
	CharSelection selection = m_selection;

	if ( m_lineUpCount == 0 )
		m_lineUpStart = selection.endPoint;

	int count = up
	            ? m_lineUpCount + 1
	            : m_lineUpCount - 1;

	int y = m_lineUpStart.y - ( m_styleRegistry.LineHeight() * count );

	if ( 0 <= y && y < m_blocks.Height() )
	{
		m_lineUpCount = count;
		selection.endPoint.y = y;
		selection.endPoint.x = m_lineUpStart.x;
		selection.end = m_blocks.CharFromPoint( &selection.endPoint );
	}

	if ( !moveSelection )
		selection.start = selection.end;

	MoveSelection( selection, true );
}

void TextView::Backspace( bool wholeWord )
{
	CharSelection selection = m_selection;

	if ( selection.IsEmpty() )
	{
		if ( selection.end == 0 )
			return;

		selection.end = wholeWord
		                   ? m_doc.PrevWordStop( selection.end - 1 )
		                   : selection.end - 1;
	}

	Clear( selection );
}

void TextView::Delete( bool wholeWord )
{
	CharSelection selection = m_selection;

	if ( selection.IsEmpty() )
	{
		if ( selection.end == m_doc.Length() )
			return;

		selection.end = wholeWord
		                   ? m_doc.NextWordStop( selection.end )
		                   : m_doc.NextCharStop( selection.end );
	}

	Clear( selection );
}

std::wstring TextView::GetText()
{
	std::wstring text;

	if ( !m_doc.Empty() )
	{
		text.resize( m_doc.Length() );
		m_doc.Read( 0, m_doc.Length(), &text[0] );
	}

	return text;
}

void TextView::SetText( ArrayRef<const wchar_t> text )
{
	m_doc.SetBeforeSelection( m_selection );
	m_doc.EndUndoGroup();
	m_lastEditOperation = lastWasNothing;

	CharChange change;
	change += m_doc.Delete( 0, m_doc.Length() );
	change += m_doc.Insert( 0, text );

	CharSelection selection;
	UpdateLayout( change, selection );
}

void TextView::Insert( ArrayRef<const wchar_t> text )
{
	//m_doc.SetBeforeSelection( m_selection );

	//if ( m_lastEditOperation != lastWasInsert )
	//{
	//	m_doc.EndUndoGroup();
	//	m_lastEditOperation = lastWasInsert;
	//}

	CharChange change;
	change += m_doc.Delete( m_selection.Min(), m_selection.Length() );
	change += m_doc.Insert( m_selection.Min(), text );

	CharSelection selection;
	selection.start = m_selection.Max() + change.delta;
	selection.end   = selection.start;

	UpdateLayout( change, selection );
}

void TextView::SelectAll()
{
	CharSelection selection;
	selection.start    = 0;
	selection.end      = m_doc.Length();
	selection.endPoint = m_blocks.PointFromChar( selection.end, true );

	MoveSelection( selection, true );
}

void TextView::SelectWord()
{
	std::pair<size_t, size_t> word = m_doc.WordAt( m_selection.end );

	CharSelection selection;
	selection.start    = word.first;
	selection.end      = word.second;
	selection.endPoint = m_blocks.PointFromChar( selection.end, true );

	MoveSelection( selection, true );
}

void TextView::Clear( CharRange rangeToClear )
{
	if ( rangeToClear. )
		return;

	if ( m_lastEditOperation != lastWasClear )
	{
		m_doc.NewUndoGroup();
		m_lastEditOperation = lastWasClear;
	}

	CharChange change = m_doc.Delete( rangeToClear.Min(), rangeToClear.Size(), m_selection );

	CharRange newSelection( rangeToClear.Min(), selection.start );

	UpdateLayout( change, newSelection );
}

void TextView::Cut()
{
	m_doc.NewUndoGroup();

	Copy();
	Clear( m_selection );
}

void TextView::Copy()
{
	if ( m_selection.IsEmpty() )
		return;
	
	if ( !OpenClipboard( NULL ) )
		return;

	EmptyClipboard();

	size_t sizeWithCRLF = m_doc.SizeWithCRLF( m_selection.Min(), m_selection.Size() );
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE, ( sizeWithCRLF + 1 ) * sizeof( wchar_t ) );

	if ( hGlobal != 0 )
	{
		wchar_t* dest = static_cast<wchar_t*>( GlobalLock( hGlobal ) );

		if ( dest != 0 )
		{
			m_doc.ReadWithCRLF( m_selection.Min(), m_selection.Size(), ArrayRef<wchar_t>( dest, dest + sizeWithCRLF ) );
			dest[sizeWithCRLF] = wchar_t( 0 );

			GlobalUnlock( hGlobal );

			if ( !SetClipboardData( CF_UNICODETEXT, hGlobal ) )
				GlobalFree( hGlobal );
		}
	}

	CloseClipboard();
}

void TextView::Paste()
{
	m_doc.EndUndoGroup();

	if ( !IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		return;

	if ( !OpenClipboard( m_hwnd ) )
		return;

	HGLOBAL hClipboardData = GetClipboardData( CF_UNICODETEXT );

	if ( hClipboardData != 0 )
	{
		wchar_t* clipboardString = static_cast<wchar_t*>( GlobalLock( hClipboardData ) );

		if ( clipboardString != 0 )
			Insert( ArrayRef<const wchar_t>( clipboardString, wcslen( clipboardString ) ) );

		GlobalUnlock( hClipboardData );
	}

	CloseClipboard();
}

void TextView::Undo()
{
	if ( !m_doc.CanUndo() )
		return;

	std::pair<TextChange,CharSelection> change = m_doc.Undo();
	UpdateLayout( change.first, change.second );
}

void TextView::Redo()
{
	if ( !m_doc.CanRedo() )
		return;

	TextChange change = m_doc.Redo();

	CharSelection selection;
	selection.start = change.end + change.delta;
	selection.end   = selection.start;

	UpdateLayout( change, selection );
}

size_t TextView::LineNumberDigits()
{
	size_t power = 10;

	for ( size_t digits = 1; digits < 10; ++digits )
	{
		if ( m_doc.LineCount() < power )
			return digits;

		power *= 10;
	}

	return 1;
}

bool TextView::AdjustGutterWidth()
{
	int oldGutterWidth = m_metrics.gutterWidth;
	m_metrics.gutterWidth = std::max<int>( 5, LineNumberDigits() + 2 ) * m_styleRegistry.AvgCharWidth();
	return oldGutterWidth != m_metrics.gutterWidth;
}

void TextView::UpdateLayout( TextChange change, CharSelection selection )
{
	if ( m_styleRegistry.Annotator() )
	{
		m_styleRegistry.Annotator()->TextChanged( change );

		if ( m_selection != selection )
			m_styleRegistry.Annotator()->SelectionChanged( selection.start, selection.end );
	}

	if ( AdjustGutterWidth() )
		change.AddChange( TextChange( 0, m_doc.Length(), TextChange::modification ) );

	RECT rect = m_metrics.TextRect( m_hwnd );

	HDC hdc = GetDC( m_hwnd );
	m_blocks.Update( hdc, rect.right - rect.left, change );
	ReleaseDC( m_hwnd, hdc );

	selection.endPoint = m_blocks.PointFromChar( selection.end, true );
	m_selection = selection;

	ScrollToCaret();
	UpdateCaretPos();

	InvalidateRect( m_hwnd, NULL, FALSE );
}

void TextView::UpdateLayout()
{
	TextChange change( 0, m_doc.Length(), TextChange::modification );
	UpdateLayout( change, m_selection );
}

void TextView::MoveSelection( CharSelection selection, bool scroll )
{
	if ( selection == m_selection )
		return;

	m_doc.EndUndoGroup();

	if ( m_styleRegistry.Annotator() )
		m_styleRegistry.Annotator()->SelectionChanged( selection.start, selection.end );

	m_selection = selection;

	if ( scroll )
		ScrollToCaret();

	UpdateCaretPos();
	InvalidateRect( m_hwnd, NULL, TRUE );
}

void TextView::ScrollToCaret()
{
	RECT textRect = m_metrics.ClientToText( m_metrics.TextRect( m_hwnd ) );

	int textHeight         = textRect.bottom - textRect.top;
	int adjustedTextHeight = textHeight - textHeight % m_styleRegistry.LineHeight();

	int xOffset = m_metrics.xOffset;
	int yOffset = m_metrics.yOffset;

	if ( m_selection.endPoint.y + m_styleRegistry.LineHeight() > textRect.bottom )
		yOffset = m_selection.endPoint.y + m_styleRegistry.LineHeight() - adjustedTextHeight;
	else if ( m_selection.endPoint.y < textRect.top )
		yOffset = m_selection.endPoint.y;

	if ( xOffset != m_metrics.xOffset || yOffset != m_metrics.yOffset )
		ScrollTo( xOffset, yOffset );
}

void TextView::UpdateCaretPos()
{
	POINT point = m_metrics.TextToClient( m_selection.endPoint );

	RECT rect = { point.x, point.y, point.x + m_metrics.caretWidth, point.y + m_styleRegistry.LineHeight() };
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
	y = (std::min)( y, int( ( m_blocks.LineCount() - m_metrics.linesPerPage ) * m_styleRegistry.LineHeight() ) );
	y = (std::max)( y, int( 0 ) );

	ScrollWindowEx( m_hwnd, 0, m_metrics.yOffset - y, NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE );
	m_metrics.yOffset = y;

	SCROLLINFO si = { sizeof si };
	si.fMask      = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
	si.nPage      = m_metrics.linesPerPage * m_styleRegistry.LineHeight();
	si.nMin       = 0;
	si.nMax       = m_blocks.LineCount() * m_styleRegistry.LineHeight() - 1;
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

	ScrollDelta( 0, -dy * m_styleRegistry.LineHeight() );
}

void TextView::OnVScroll( UINT code )
{
	switch ( code )
	{
	case SB_LINEUP:         ScrollDelta( 0, -m_styleRegistry.LineHeight() ); break;
	case SB_LINEDOWN:       ScrollDelta( 0,  m_styleRegistry.LineHeight() ); break;
	case SB_PAGEUP:         ScrollDelta( 0, -m_metrics.linesPerPage * m_styleRegistry.LineHeight() ); break;
	case SB_PAGEDOWN:       ScrollDelta( 0,  m_metrics.linesPerPage * m_styleRegistry.LineHeight() ); break;
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
	return m_styleRegistry.LineHeight() * ( y / m_styleRegistry.LineHeight() );
}

HWND TextView::WindowHandle() const
{
	return m_hwnd;
}
