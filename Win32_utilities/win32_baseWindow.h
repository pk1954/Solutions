// win32_baseWindow.h : 
//

#pragma once

#include "win32_rootWindow.h"

class BaseWindow : public RootWindow
{
public:

    BaseWindow( );
	~BaseWindow( );

    HWND StartBaseWindow( HWND const, UINT const, LPCTSTR const, DWORD const );

private:
    LPCTSTR m_szClass;	

    virtual LRESULT UserProc( UINT const, WPARAM const, LPARAM const ) = 0;

friend static LRESULT CALLBACK BaseWndProc( HWND const, UINT const, WPARAM const, LPARAM const );
};
