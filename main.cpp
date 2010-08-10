// main.cpp

#include "TextEdit.h"
#include <windows.h>
#include <uxtheme.h>

#define APPCLASS TEXT( "GoodNewsEveryone" )

HINSTANCE theInstance = 0;
HWND      theEdit     = 0;

void OnSize( HWND hwnd )
{
	RECT rect;
	GetClientRect( hwnd, &rect );
	MoveWindow( theEdit, 0, 0, rect.right, rect.bottom, TRUE );
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_CREATE:
		theEdit = CreateWindow( WC_TEXTEDIT, TEXT( "" ), WS_CHILD|WS_VISIBLE|WS_VSCROLL, 0, 0, 0, 0, hwnd, NULL, theInstance, NULL );
		return theEdit ? 0 : -1;

	case WM_SIZE:
		OnSize( hwnd );
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hwnd, msg, wParam, lParam );
}

bool InitApp()
{
	WNDCLASSEX wcx      = { sizeof wcx };
	wcx.lpfnWndProc     = WndProc;
	wcx.hInstance       = theInstance;
	wcx.hCursor         = LoadCursor( NULL, IDC_ARROW );
	wcx.hIcon           = LoadIcon( NULL, IDI_APPLICATION );
	wcx.hbrBackground   = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
	wcx.lpszClassName   = APPCLASS;

	return RegisterClassEx( &wcx ) ? true : false;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	theInstance = hInstance;

	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	BufferedPaintInit();

	if ( !InitApp() || !RegisterTextEdit( hInstance ) )
		return 0;

	HWND hwnd = CreateWindow( APPCLASS,
	                          TEXT( "Let's edit some shit, yeah?" ),
	                          WS_OVERLAPPEDWINDOW,
	                          CW_USEDEFAULT, CW_USEDEFAULT,
	                          CW_USEDEFAULT, CW_USEDEFAULT,
	                          NULL,
	                          NULL,
	                          theInstance,
	                          0 );

	ShowWindow( hwnd, nShowCmd );

	MSG msg;
	while ( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage ( &msg );
	}

	BufferedPaintUnInit();
	CoUninitialize();

	return 0;
}
