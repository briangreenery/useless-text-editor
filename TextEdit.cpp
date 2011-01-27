// TextEdit.cpp

#include "TextEdit.h"
#include "TextView.h"
#include <Windowsx.h>

static inline POINT MakePOINT( LPARAM lParam )
{
	POINT point = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
	return point;
}

static LRESULT ProcessWindowMsg( TextView* view, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_CREATE:
		return view->OnCreate( reinterpret_cast<LPCREATESTRUCT>( lParam ) );

	case WM_PAINT:
		view->OnPaint();
		return 0;

	case WM_SIZE:
		view->OnSize( wParam, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
		return 0;

	case WM_CHAR:
		view->OnChar( wParam, LOWORD( lParam ), HIWORD( lParam ) );
		return 0;

	case WM_KEYDOWN:
		view->OnKeyDown( wParam, LOWORD( lParam ), HIWORD( lParam ) );
		return 0;

	case WM_LBUTTONDBLCLK:
		view->OnLButtonDblClk( MakePOINT( lParam ) );
		return 0;

	case WM_LBUTTONDOWN:
		view->OnLButtonDown( MakePOINT( lParam ) );
		return 0;

	case WM_LBUTTONUP:
		view->OnLButtonUp( MakePOINT( lParam ) );
		return 0;

	case WM_MOUSEMOVE:
		view->OnMouseMove( MakePOINT( lParam ) );
		return 0;

	case WM_CAPTURECHANGED:
		view->OnCaptureChanged();
		return 0;

	case WM_MOUSEACTIVATE:
		return view->OnMouseActivate();

	case WM_SETCURSOR:
		view->OnSetCursor();
		return TRUE;

	case WM_SETFOCUS:
		view->OnSetFocus( reinterpret_cast<HWND>( wParam ) );
		return 0;

	case WM_KILLFOCUS:
		view->OnKillFocus( reinterpret_cast<HWND>( wParam ) );
		return 0;

	case WM_VSCROLL:
		view->OnVScroll( LOWORD( wParam ) );
		return 0;

	case WM_HSCROLL:
		view->OnHScroll( LOWORD( wParam ) );
		return 0;

	case WM_MOUSEWHEEL:
		view->OnMouseWheel( (short) HIWORD( wParam ) );
		return 0;

	case WM_TIMER:
		view->OnTimer( static_cast<UINT_PTR>( wParam ) );
		return 0;
	}

	return DefWindowProc( hwnd, msg, wParam, lParam );
}

LRESULT CALLBACK TextEditWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( msg == WM_NCCREATE )
	{
		try
		{
			SetWindowLongPtr( hwnd, 0, reinterpret_cast<LONG_PTR>( new TextView( hwnd ) ) );
			return TRUE;
		}
		catch ( std::bad_alloc& )
		{
			return FALSE;
		}
	}

	TextView* view = reinterpret_cast<TextView*>( GetWindowLongPtr( hwnd, 0 ) );

	if ( msg == WM_NCDESTROY )
	{
		delete view;
		SetWindowLongPtr( hwnd, 0, 0 );
		return 0;
	}

	if ( view != 0 )
		return ProcessWindowMsg( view, hwnd, msg, wParam, lParam );

	return DefWindowProc( hwnd, msg, wParam, lParam );
}

extern "C"
{
	BOOL RegisterTextEdit( HINSTANCE hInstance )
	{
		WNDCLASSEX wcx     = { sizeof wcx };
		wcx.style          = CS_DBLCLKS;
		wcx.lpfnWndProc    = TextEditWndProc;
		wcx.cbWndExtra     = sizeof( TextView* );
		wcx.hInstance      = hInstance;
		wcx.lpszClassName  = WC_TEXTEDIT;

		return RegisterClassEx( &wcx ) ? TRUE : FALSE;
	}
}
