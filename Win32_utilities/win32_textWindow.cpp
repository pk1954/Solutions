// win32_textWindow.cpp :
//

#include "stdafx.h"
#include <locale>
#include <iomanip>
#include "win32_util.h"
#include "win32_thread.h"
#include "win32_util_resource.h"
#include "win32_textWindowThread.h"
#include "win32_textWindow.h"

TextWindow::TextWindow( ) :  
    BaseWindow( ),
	m_pTextWindowThread( nullptr ),
	m_pRefreshTimer( nullptr ),
	m_hDC_Memory( 0 ),
	m_hBitmap( 0 )
{ }

void TextWindow::StartTextWindow
(
    HWND                  const   hwndParent,
	PixelRect             const & rect,
	LPCTSTR               const   szClass,
    UINT                  const   uiAlpha,
	BOOL                  const   bAsync,
	std::function<bool()> const   visibilityCriterion
)
{
    HWND const hwnd = StartBaseWindow
    ( 
        hwndParent,
        CS_OWNDC | CS_DBLCLKS,
        szClass,
        WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_CAPTION,
		& rect,
		visibilityCriterion
    );

	HDC const hDC = GetDC( hwnd );   assert( hDC != nullptr );
	m_hDC_Memory = CreateCompatibleDC( hDC );
	m_hBitmap    = CreateCompatibleBitmap( hDC );
	SelectObject( m_hDC_Memory, m_hBitmap );
	ReleaseDC( hwnd, hDC );
	Util::MakeLayered( hwnd, TRUE, 0, uiAlpha );
    SetWindowText( hwnd, szClass );

	m_pTextWindowThread = new TextWindowThread
	( 
		m_hDC_Memory, 
		PixelRectSize{ rect.GetSize() }, 
		this, 
		szClass, 
		bAsync 
	);
}

void TextWindow::StopTextWindow( )
{
	m_pTextWindowThread->Terminate( );
	delete m_pTextWindowThread;
	m_pTextWindowThread = nullptr;

	DeleteObject( m_hBitmap );
	m_hBitmap = 0;

	DeleteDC( m_hDC_Memory );
	m_hDC_Memory = 0;
}

void TextWindow::AddContextMenuEntries( HMENU const hPopupMenu, PixelPoint const pntPos )
{
    UINT const STD_FLAGS = MF_BYPOSITION | MF_STRING;

    (void)AppendMenu( hPopupMenu, STD_FLAGS, IDM_HIDE_WINDOW, L"Hide window" );
}

void TextWindow::Trigger( )
{
	if ( m_pTextWindowThread )
		m_pTextWindowThread->Trigger( );
}

LRESULT TextWindow::UserProc( UINT const message, WPARAM const wParam, LPARAM const lParam )
{
    switch (message)
    {

    case WM_COMMAND:
    {
        int const wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_HIDE_WINDOW:
	        Show( FALSE );
            break;

        default:
            break;
        }
    }
    return FALSE;

    case WM_PAINT:
    {
        PAINTSTRUCT   ps;
        HDC           hDC      = BeginPaint( &ps );
		PixelRectSize rectSize = GetClRectSize( );
		BitBlt( hDC, 0, 0, rectSize.GetXvalue(), rectSize.GetYvalue(), m_hDC_Memory, 0, 0, SRCCOPY );
        (void)EndPaint( &ps );
        return FALSE;
    }

    case WM_CLOSE:    // Do not destroy, just hide  
        Show( FALSE );
        return TRUE;

    default:
        break;
    }
    
    return DefWindowProc( message, wParam, lParam );
}
