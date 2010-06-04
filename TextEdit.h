// TextEdit.h

#ifndef TextEdit_h
#define TextEdit_h

#include <Windows.h>

#define WC_TEXTEDIT TEXT( "IamAtextEditor" )

extern "C"
{
	BOOL RegisterTextEdit( HINSTANCE );
}

#endif
