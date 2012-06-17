// main.cpp

#include "TextEdit.h"
#include <windows.h>
#include <uxtheme.h>
#include <GdiPlus.h>

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

#include "TextChangeTest.h"

static void RunTests()
{
	TextChangeTest();
}

#include <regex>

static void OneOff()
{
	std::string test = "(?<=\\.)";//"(s(ystemLanguage|cr(ipts|ollbars|een(X|Y|Top|Left))|t(yle(Sheets)?|atus(Text|bar)?)|ibling(Below|Above)|ource|uffixes|e(curity(Policy)?|l(ection|f)))|h(istory|ost(name)?|as(h|Focus))|y|X(MLDocument|SLDocument)|n(ext|ame(space(s|URI)|Prop))|M(IN_VALUE|AX_VALUE)|c(haracterSet|o(n(structor|trollers)|okieEnabled|lorDepth|mp(onents|lete))|urrent|puClass|l(i(p(boardData)?|entInformation)|osed|asses)|alle(e|r)|rypto)|t(o(olbar|p)|ext(Transform|Indent|Decoration|Align)|ags)|SQRT(1_2|2)|i(n(ner(Height|Width)|put)|ds|gnoreCase)|zIndex|o(scpu|n(readystatechange|Line)|uter(Height|Width)|p(sProfile|ener)|ffscreenBuffering)|NEGATIVE_INFINITY|d(i(splay|alog(Height|Top|Width|Left|Arguments)|rectories)|e(scription|fault(Status|Ch(ecked|arset)|View)))|u(ser(Profile|Language|Agent)|n(iqueID|defined)|pdateInterval)|_content|p(ixelDepth|ort|ersonalbar|kcs11|l(ugins|atform)|a(thname|dding(Right|Bottom|Top|Left)|rent(Window|Layer)?|ge(X(Offset)?|Y(Offset)?))|r(o(to(col|type)|duct(Sub)?|mpter)|e(vious|fix)))|e(n(coding|abledPlugin)|x(ternal|pando)|mbeds)|v(isibility|endor(Sub)?|Linkcolor)|URLUnencoded|P(I|OSITIVE_INFINITY)|f(ilename|o(nt(Size|Family|Weight)|rmName)|rame(s|Element)|gColor)|E|whiteSpace|l(i(stStyleType|n(eHeight|kColor))|o(ca(tion(bar)?|lName)|wsrc)|e(ngth|ft(Context)?)|a(st(M(odified|atch)|Index|Paren)|yer(s|X)|nguage))|a(pp(MinorVersion|Name|Co(deName|re)|Version)|vail(Height|Top|Width|Left)|ll|r(ity|guments)|Linkcolor|bove)|r(ight(Context)?|e(sponse(XML|Text)|adyState))|global|x|m(imeTypes|ultiline|enubar|argin(Right|Bottom|Top|Left))|L(N(10|2)|OG(10E|2E))|b(o(ttom|rder(Width|RightWidth|BottomWidth|Style|Color|TopWidth|LeftWidth))|ufferDepth|elow|ackground(Color|Image)))\b";

	std::regex regex( test, std::regex::ECMAScript|std::regex::nosubs );

	int x = 1;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	theInstance = hInstance;

	RunTests();
	//OneOff();

	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );

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
	Gdiplus::GdiplusShutdown( gdiplusToken );

	return 0;
}
