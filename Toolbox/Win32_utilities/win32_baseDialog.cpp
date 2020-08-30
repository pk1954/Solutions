// win32_baseDialog.cpp
//

#include "stdafx.h"
#include "win32_util.h"
#include "win32_rootWindow.h"
#include "win32_baseDialog.h"

HWND BaseDialog::StartBaseDialog
(
    HWND             const hwndParent,
    LPCTSTR          const lpTemplateName,
	function<bool()> const visibilityCriterion
)
{
	HWND hwnd = CreateDialogParam
    (
        nullptr,
        lpTemplateName,
        hwndParent,
        BaseDialogProc,
        ( LPARAM )this
    );

	SetWindowHandle( hwnd );
	StartRootWindow( visibilityCriterion );

	return hwnd;
}

static INT_PTR CALLBACK BaseDialogProc
( 
    HWND   const hwnd,
    UINT   const message, 
    WPARAM const wParam, 
    LPARAM const lParam 
)
{
	if ( message == WM_INITDIALOG )
	{
		SetUserDataPtr( hwnd, (LONG_PTR)lParam );
		return true;
	}
	else 
	{
		BaseDialog * pBaseDialog = reinterpret_cast<BaseDialog *>(GetUserDataPtr( hwnd ));
		if ( RootWinIsReady( pBaseDialog ) )
			return pBaseDialog->RootWindowProc( hwnd, message, wParam, lParam );         // normal case
	}

	return false;
}
