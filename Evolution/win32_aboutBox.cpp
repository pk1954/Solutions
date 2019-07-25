// win32_aboutBox.cpp :
//

#include "stdafx.h"
#include "resource.h"
#include "win32_aboutBox.h"

static char COMPILE_TIMESTAMP[] = __DATE__ " " __TIME__;

static INT_PTR CALLBACK About
( 
    HWND   const hDlg, 
    UINT   const message, 
    WPARAM const wParam, 
    LPARAM const lParam 
)
{
    UNREFERENCED_PARAMETER( lParam );

    switch (message)
    {

	case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            (void)EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;

    default:
        break;
    }

    return FALSE;
}

void ShowAboutBox( HWND const hwndParent ) 
{
    HINSTANCE const hInstance = GetModuleHandle( nullptr );
    (void)DialogBox( hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwndParent, About );
}
