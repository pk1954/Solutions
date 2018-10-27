// win32_refreshRateDialog.cpp :
//

#include "stdafx.h"
#include <wchar.h>
#include <stdio.h>
#include "win32_util_resource.h"
#include "win32_refreshRateDialog.h"

using namespace std;

static int const BUFLEN = 20;
static wchar_t m_wBuffer[BUFLEN];
static int     m_iRefreshRate; // in milliseconds

static INT_PTR CALLBACK dialogProc
( 
    HWND   const hDlg, 
    UINT   const message, 
    WPARAM const wParam, 
    LPARAM const lParam 
)
{
    switch (message)
    {
    case WM_INITDIALOG:
	{
		swprintf_s( m_wBuffer, BUFLEN, L"%d", m_iRefreshRate );
		(void)::SendMessage
		( 
			GetDlgItem( hDlg, IDD_REFRESH_RATE_EDIT_CTL ), 
			WM_SETTEXT,
			0, 
			(LPARAM)( m_wBuffer )
		);
        return TRUE;
	}

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
			if ( GetWindowText( GetDlgItem( hDlg, IDD_REFRESH_RATE_EDIT_CTL ), m_wBuffer, BUFLEN ) )
			{
				int iRate;
				if ( swscanf_s( m_wBuffer, L"%d", &iRate ) > 0 )
				{
					m_iRefreshRate = iRate;
					EndDialog( hDlg, LOWORD(wParam) );
				}
				else
				{
					MessageBeep( MB_ICONWARNING );
				}
			}

            return TRUE;
        }
        break;

    default:
        break;
    }

    return FALSE;
}

int RefreshRateDialog::Show( HWND const hwndParent, int iRefreshRate )
{
	m_iRefreshRate = iRefreshRate;

    DialogBox( NULL, MAKEINTRESOURCE(IDD_REFRESH_RATE_DIALOG), hwndParent, dialogProc );

	return m_iRefreshRate;
}