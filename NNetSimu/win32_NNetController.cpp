// win32_NNetController.cpp
//
// NNetSimu

#include "stdafx.h"
#include "Windowsx.h"
#include "Windows.h"
#include "Resource.h"
#include "BoolOp.h"
#include "win32_speedControl.h"
#include "win32_zoomControl.h"
#include "win32_aboutBox.h"
#include "win32_NNetAppWindow.h"
#include "win32_NNetWindow.h"
#include "win32_NNetEditor.h"
#include "win32_NNetWorkThreadInterface.h"
#include "win32_NNetController.h"

NNetController::NNetController
(
	WinManager       * const pWinManager,
	NNetWindow       * const pNNetWindow,
	StatusBar        * const pStatusBar,
	NNetEditorWindow * const pNNetEditorWindow
) : 
	m_pAppWindow              ( nullptr ),
	m_pNNetWorkThreadInterface( nullptr ),
	m_pWinManager             ( pWinManager ),
    m_pDelay                  ( nullptr ),
	m_pStatusBar              ( pStatusBar ),
	m_pNNetWindow             ( pNNetWindow ),
	m_pNNetEditorWindow       ( pNNetEditorWindow ),
	m_hCrsrWait               ( 0 )
{ }

NNetController::~NNetController( )
{
	m_pNNetWorkThreadInterface = nullptr;
	m_pAppWindow               = nullptr;
	m_pWinManager              = nullptr;
	m_pDelay                   = nullptr;
    m_pStatusBar               = nullptr;
}

void NNetController::Initialize
( 
	NNetAppWindow           * const pAppWindow,
	NNetWorkThreadInterface * const pNNetWorkThreadInterface,
	Delay                   * const pDelay
)
{
	m_pNNetWorkThreadInterface = pNNetWorkThreadInterface;
	m_pAppWindow               = pAppWindow;
    m_pDelay                   = pDelay;
	m_hCrsrWait                = LoadCursor( NULL, IDC_WAIT );
}

bool NNetController::ProcessUIcommand( int const wmId, LPARAM const lParam )
{
	switch (wmId)
	{

	case IDM_MAX_SPEED:
		{
			HWND hwndStatusBar = m_pStatusBar->GetWindowHandle( );
			m_pStatusBar->SetTrackBarPos( IDM_SIMULATION_SPEED, MAX_DELAY );                
			EnableWindow( GetDlgItem( hwndStatusBar, IDM_MAX_SPEED ), FALSE );
			m_pDelay->SetDelay( 0 );
		}
		break;

	case IDM_TRACKBAR:
		switch ( lParam )
		{
		case IDM_ZOOM_TRACKBAR:
		{
			LONG const lLogicalPos = m_pStatusBar->GetTrackBarPos( IDM_ZOOM_TRACKBAR );
			LONG const lValue      = lLogicalPos;
			LONG const lPos        = LogarithmicTrackbar::TrackBar2ValueL( lValue );
			ProcessUIcommand( IDM_ZOOM_TRACKBAR, lPos );
		}
		break;

		case IDM_SIMULATION_SPEED:
		{
			LONG const lLogicalPos = m_pStatusBar->GetTrackBarPos( IDM_SIMULATION_SPEED );
			LONG const lValue      = LogarithmicTrackbar::Value2TrackbarL( MAX_DELAY ) - lLogicalPos;
			LONG const lPos        = LogarithmicTrackbar::TrackBar2ValueL( lValue );
			EnableWindow( m_pStatusBar->GetDlgItem( IDM_MAX_SPEED ), TRUE );
			m_pDelay->SetDelay( lPos );
		}
			break;

		default:
			assert( false );
		}
		break;

	case IDM_ZOOM_TRACKBAR:  // comes from trackbar in statusBar
		(void)m_pNNetWindow->SetPixelSize( NanoMeter(CastToShort(lParam)) );
		break;

	case IDM_FIT_ZOOM:
		//m_pNNetWindow->Fit2Rect( );
		//setSizeTrackBar( m_pNNetWindow->GetPixelSize() );
		break;

	case IDM_ZOOM_OUT:
	case IDM_ZOOM_IN:
		m_pNNetWindow->Zoom( wmId == IDM_ZOOM_IN );
		setSizeTrackBar( m_pNNetWindow->GetPixelSize() );
		break;

	case IDM_SET_ZOOM:
		m_pNNetWindow->SetPixelSize( NanoMeter( static_cast<double>(lParam) ) );
		setSizeTrackBar( NanoMeter(CastToLong(lParam)) );
		break;

	case IDM_REFRESH:
		m_pNNetWindow->Refresh();
		break;

	default:
		return FALSE; // command has not been processed
	}

	return TRUE;  // command has been processed
}

bool NNetController::ProcessModelCommand( int const wmId, LPARAM const lParam )
{
	switch ( wmId )
	{

	default:
		return true;  // Some commands are handled by the framework controller
		break;
	}
}

void NNetController::setSizeTrackBar( NanoMeter const nmPixelSize )
{ 
	m_pStatusBar->SetTrackBarPos( IDM_ZOOM_TRACKBAR, CastToLong( LogarithmicTrackbar::Value2TrackbarD( nmPixelSize.GetValue() ) ) ); 
}
