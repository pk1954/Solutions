// win32_rootWindow.cpp
//
// Toolbox Win32_utilities

#include "stdafx.h"
#include "PixelTypes.h"
#include "win32_util_resource.h"
#include "win32_baseRefreshRate.h"
#include "win32_rootWindow.h"

using namespace std::chrono;

bool RootWinIsReady( RootWindow const * pRootWin )
{
    return ( ( pRootWin != nullptr ) && ( pRootWin->GetWindowHandle( ) != nullptr ) );
}

class RootWindow::WindowRefreshRate : public BaseRefreshRate
{
public:
	WindowRefreshRate( RootWindow * const pRootWin )
		: m_pRootWin( pRootWin )
	{ }

	virtual void Trigger( )
	{
		m_pRootWin->Trigger( );
	}

private:
	RootWindow * const m_pRootWin;
};

RootWindow::RootWindow( )
{
	m_pRefreshRate = new WindowRefreshRate( this );
}

RootWindow::~RootWindow( ) 
{ 
	delete m_pRefreshRate;
	m_pRefreshRate = nullptr;
	m_hwnd         = nullptr; 
}

void RootWindow::StartRootWindow( function<bool()> const visibilityCriterion )
{
	m_visibilityCriterion = visibilityCriterion;

	m_visibilityMode = m_visibilityCriterion 
		? tOnOffAuto::automatic 
		: IsWindowVisible( ) 
          ? tOnOffAuto::on 
		  : tOnOffAuto::off;
}

void RootWindow::addWinMenu( HMENU const hMenuParent, std::wstring const strTitle ) const
{
	UINT  const STD_FLAGS = MF_BYPOSITION | MF_STRING;
	HMENU const hMenu = CreatePopupMenu();
	(void)AppendMenu( hMenu, STD_FLAGS, IDM_WINDOW_AUTO, L"auto" );
	(void)AppendMenu( hMenu, STD_FLAGS, IDM_WINDOW_ON,   L"on"   );
	(void)AppendMenu( hMenu, STD_FLAGS, IDM_WINDOW_OFF,  L"off"  );
	(void)AppendMenu( hMenuParent, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hMenu, strTitle.c_str() );
}

void RootWindow::adjustWinMenu( HMENU const hMenu ) const
{
	EnableMenuItem( hMenu, IDM_WINDOW_AUTO, ((m_visibilityMode == tOnOffAuto::automatic) ? MF_GRAYED : MF_ENABLED) );
	EnableMenuItem( hMenu, IDM_WINDOW_ON,   ((m_visibilityMode == tOnOffAuto::on       ) ? MF_GRAYED : MF_ENABLED) );
	EnableMenuItem( hMenu, IDM_WINDOW_OFF,  ((m_visibilityMode == tOnOffAuto::off      ) ? MF_GRAYED : MF_ENABLED) );
}

void RootWindow::contextMenu( PixelPoint const & crsrPosScreen ) // crsr pos in screen coordinates
{
	HMENU const hPopupMenu { CreatePopupMenu() };

	long lParam = AddContextMenuEntries( hPopupMenu ); // arbitrary parameter, forwarded as lParam  

	if ( m_visibilityCriterion )
	{
		addWinMenu( hPopupMenu, L"Show window" );
		adjustWinMenu( hPopupMenu );
	}

	if ( m_bShowRefreshRateDlg && (m_pRefreshRate->GetRefreshRate( ) > 0ms) )
	{
		(void)AppendMenu( hPopupMenu, MF_STRING, IDD_REFRESH_RATE_DIALOG, L"Window refresh rate" );
	}

	(void)SetForegroundWindow( GetWindowHandle( ) );

	UINT const uiID = (UINT)TrackPopupMenu
	( 
		hPopupMenu, 
		TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, 
		crsrPosScreen.GetXvalue(), crsrPosScreen.GetYvalue(), 
		0, 
		GetWindowHandle( ),
		nullptr 
	);        

	(void)DestroyMenu( hPopupMenu );

	if ( uiID != 0 )
		OnCommand( uiID, lParam, Screen2Client( crsrPosScreen ) );
}

void RootWindow::SetWindowHandle( HWND const hwnd ) 
{ 
	assert( hwnd );
	m_hwnd    = hwnd;  
	m_hwndApp = GetAncestor( m_hwnd, GA_ROOTOWNER );
};

void RootWindow::SetRefreshRate( milliseconds const msRate ) 
{ 
	m_pRefreshRate->SetRefreshRate( msRate ); 
}

void RootWindow::Notify( bool const bImmediately )
{
	m_pRefreshRate->Notify( bImmediately );
}

void RootWindow::SetTrackBarPos( INT const idTrackbar, LONG const lPos ) const
{
	(void)SendDlgItemMessage
	(   
		idTrackbar, TBM_SETPOS, 
		static_cast<WPARAM>( true ),                   // redraw flag 
		static_cast<LPARAM>( lPos )
	); 
}

void RootWindow::SetTrackBarRange( INT const idTrackbar, LONG const lMin, LONG const lMax ) const
{
	(void)SendDlgItemMessage
	( 
		idTrackbar, 
		TBM_SETRANGEMIN, 
		static_cast<WPARAM>( false ),                   // redraw flag 
		static_cast<LPARAM>( lMin ) 
	);
	(void)SendDlgItemMessage
	( 
		idTrackbar, 
		TBM_SETRANGEMAX, 
		static_cast<WPARAM>( true ),                   // redraw flag 
		static_cast<LPARAM>( lMax ) 
	);
}

bool RootWindow::OnCommand( WPARAM const wParam, LPARAM const lParam, PixelPoint const pixPoint )
{
	UINT const uiCmdId  = LOWORD( wParam );

	switch ( uiCmdId )
	{
	case IDM_WINDOW_ON:
		m_visibilityMode = tOnOffAuto::on;
		Show( true );
		break;

	case IDM_WINDOW_OFF:
		m_visibilityMode = tOnOffAuto::off;
		Show( false );
		break;

	case IDM_WINDOW_AUTO:
		m_visibilityMode = tOnOffAuto::automatic;
		Show( ApplyAutoCriterion( tOnOffAuto::automatic, m_visibilityCriterion ) );
		break;

	case IDD_REFRESH_RATE_DIALOG:
		m_pRefreshRate->RefreshRateDialog( m_hwnd );
		break;

	default:
		return false;
	}

	return true;
}

LRESULT RootWindow::RootWindowProc
( 
	HWND   const hwnd,
	UINT   const message, 
	WPARAM const wParam, 
	LPARAM const lParam 
)
{
	RootWindow * const pRootWin = reinterpret_cast<RootWindow *>(GetWindowLongPtr( hwnd, GWLP_USERDATA ));

	switch (message)
	{

	case WM_CONTEXTMENU:
		pRootWin->contextMenu( GetCrsrPosFromLparam( lParam ) );
		return false;

	case WM_COMMAND:
		pRootWin->OnCommand( wParam, lParam );
		break;

	case WM_CLOSE:   
		pRootWin->m_visibilityMode = tOnOffAuto::off;
		break;

	default:
		break;
	}

	return pRootWin->UserProc( message, wParam, lParam );
}
