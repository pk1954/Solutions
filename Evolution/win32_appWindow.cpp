// win32_appWindow.cpp
//

#include "stdafx.h"

#include <chrono>
#include "WinUser.h"

using namespace std::literals::chrono_literals;

// EvolutionCore interfaces

#include "config.h"
#include "gridRect.h"
#include "GridDimensions.h"
#include "EvolutionCoreWrappers.h"
#include "EvolutionCore.h"

// interfaces of various windows

#include "win32_appWindow.h"
#include "win32_aboutBox.h"

// infrastructure

#include "util.h"
#include "pixelTypes.h"
#include "ObserverInterface.h"

// scripting and tracing

#include "dump.h"
#include "trace.h"
#include "errhndl.h"
#include "script.h"
#include "UtilityWrappers.h"
#include "win32_stopwatch.h"
#include "win32_wrappers.h"
#include "win32_editorWrappers.h"
#include "win32_histWrappers.h"

// system and resources

#include "Resource.h"
#include "d3d_system.h"

// application

#include "win32_resetDlg.h"
#include "win32_appWindow.h"

AppWindow::AppWindow( ) :
    BaseWindow( ),
	m_pGraphics( nullptr ),
	m_pHistorySystem( nullptr ),
	m_pModelDataWork( nullptr ),
	m_hwndConsole( nullptr ),
	m_pEvoCore4Display( nullptr ),
	m_traceStream( ),
	m_bStopped( TRUE )
{
	Stopwatch stopwatch;

	m_hCrsrWait = LoadCursor( NULL, IDC_WAIT );

	m_hwndConsole = GetConsoleWindow( );
	SetWindowPos( m_hwndConsole, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

//	_CrtSetAllocHook( MyAllocHook );

	DefineUtilityWrapperFunctions( );

	m_hwndApp = StartBaseWindow
	( 
		nullptr, 
		CS_HREDRAW | CS_VREDRAW, 
		L"ClassAppWindow", 
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		nullptr,
		nullptr
	);

	m_traceStream = OpenTraceFile( L"main_trace.out" );

	DUMP::SetDumpStream( & std::wcout );
	Config::SetDefaultConfiguration( );
    Config::DefineConfigWrapperFunctions( );
	Script::ProcessScript( L"std_configuration.in" );

    // create window objects

	stopwatch.Start();
	m_ColorManager.Initialize( );
	m_WorkThreadInterface.Initialize( & m_traceStream ); 

	stopwatch.Stop( L"create window objects" );

	m_MiniGridWindow.Observe( & m_MainGridWindow );  // mini window observes main grid window

	DefineWin32HistWrapperFunctions( & m_WorkThreadInterface );
	DefineWin32WrapperFunctions    ( & m_WorkThreadInterface );
	DefineWin32EditorWrapperFunctions( & m_EditorWindow );

    m_StatusBar     .SetRefreshRate( 300ms );
    m_EvoHistWindow .SetRefreshRate( 200ms ); 
    m_CrsrWindow    .SetRefreshRate( 100ms );
    m_Statistics    .SetRefreshRate( 100ms );
    m_PerfWindow    .SetRefreshRate( 100ms );
	m_HistInfoWindow.SetRefreshRate( 300ms );
    m_MiniGridWindow.SetRefreshRate( 300ms );
    m_MainGridWindow.SetRefreshRate( 100ms );
	
	GridWindow::InitClass
	( 
		& m_ReadBuffer, 
		& m_WorkThreadInterface, 
		& m_FocusPoint, 
		& m_DspOptWindow, 
		& m_PerfWindow, 
		& m_ColorManager 
	);

	m_CrsrWindow    .Start( m_hwndApp, & m_ReadBuffer, & m_FocusPoint );
	m_StatusBar     .Start( m_hwndApp, & m_ReadBuffer, &m_EvoHistGlue, & m_WorkThreadInterface, & m_Delay, & m_EditorWindow );
	m_Statistics    .Start( m_hwndApp, & m_ReadBuffer );
	m_HistInfoWindow.Start( m_hwndApp, nullptr );
	m_PerfWindow    .Start( m_hwndApp, m_Delay, [&](){ return m_WorkThreadInterface.IsRunning(); } );

	m_WinManager.AddWindow( L"IDM_CONS_WINDOW", IDM_CONS_WINDOW, m_hwndConsole,    TRUE,   TRUE  );
	m_WinManager.AddWindow( L"IDM_APPL_WINDOW", IDM_APPL_WINDOW, m_hwndApp,        TRUE,   TRUE  );
	m_WinManager.AddWindow( L"IDM_PERF_WINDOW", IDM_PERF_WINDOW, m_PerfWindow,     TRUE,   FALSE );
	m_WinManager.AddWindow( L"IDM_CRSR_WINDOW", IDM_CRSR_WINDOW, m_CrsrWindow,     TRUE,   FALSE );
	m_WinManager.AddWindow( L"IDM_STAT_WINDOW", IDM_STAT_WINDOW, m_Statistics,     TRUE,   FALSE );
	m_WinManager.AddWindow( L"IDM_HIST_INFO",   IDM_HIST_INFO,   m_HistInfoWindow, TRUE,   FALSE );
	m_WinManager.AddWindow( L"IDM_STATUS_BAR",  IDM_STATUS_BAR,  m_StatusBar.GetWindowHandle(), FALSE, FALSE );

	m_EvoController.Start
	( 
		& m_traceStream, 
		& m_WorkThreadInterface,
		& m_WinManager,
		& m_EvoHistGlue,
		& m_Delay, 
		& m_StatusBar, 
		& m_MainGridWindow, 
		& m_EditorWindow, 
		& m_ColorManager,
		& m_AppMenu
	);

	m_ScriptHook.Initialize( & m_StatusBar );
	Script::ScrSetWrapHook( & m_ScriptHook );

	GridDimensions::DefineGridSize
	( 
		GRID_COORD{ Config::GetConfigValueShort( Config::tId::gridWidth ) }, 
		GRID_COORD{ Config::GetConfigValueShort( Config::tId::gridHeight ) }, 
		Config::GetConfigValue( Config::tId::nrOfNeighbors ) 
	);
};

void AppWindow::Start( )
{
	EvolutionCore * pCoreWork;
	BOOL            bHexMode = (GridDimensions::GetNrOfNeigbors() == 6);

	m_AppMenu.Start( bHexMode );

	EvolutionCore::InitClass
	( 
		GridDimensions::GetNrOfNeigbors(), 
		& m_ReadBuffer, 
		& m_event
	);

	m_D3d_driver.Initialize
	( 
		m_hwndApp, 
		GridDimensions::GridWidthVal(), 
		GridDimensions::GridHeightVal(), 
		bHexMode 
	);

	m_pGraphics = & m_D3d_driver;

    m_pHistorySystem = HistorySystem::CreateHistorySystem( );  // deleted in Stop function

	m_HistInfoWindow.SetHistorySystem( m_pHistorySystem );

	m_pModelDataWork   = m_EvoHistGlue.Start( m_pHistorySystem, TRUE ); 
	pCoreWork          = m_pModelDataWork->GetEvolutionCore();
	m_pEvoCore4Display = EvolutionCore::CreateCore( );

	m_protocolServer.Start( m_pHistorySystem );
	DefineCoreWrapperFunctions( pCoreWork );  // Core wrappers run in work thread
	m_ReadBuffer.Initialize( pCoreWork, m_pEvoCore4Display );

	m_MainGridWindow.Start
	( 
		m_hwndApp, 
		m_pGraphics, 
		WS_CHILD | WS_CLIPSIBLINGS, 
		16_PIXEL, 
		nullptr
	);
    m_MiniGridWindow.Start
	( 
		m_hwndApp, 
		m_pGraphics, 
		WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_CAPTION, 
		2_PIXEL, 
		[&]() { return ! m_MainGridWindow.IsFullGridVisible( ); }
	);
	m_EvoHistWindow.Start( m_hwndApp, & m_FocusPoint, m_pHistorySystem, & m_WorkThreadInterface );
	m_DspOptWindow .Start( m_hwndApp );
    m_EditorWindow .Start( m_hwndApp, & m_WorkThreadInterface, pCoreWork, & m_DspOptWindow );
	m_FocusPoint   .Start( & m_EvoHistGlue );

	m_WorkThreadInterface.Start
	( 
		m_hwndApp, 
		& m_ColorManager, 
		& m_Delay, 
		& m_EditorWindow, 
		& m_event, 
		& m_ReadBuffer, 
		& m_EvoHistGlue 
	);
	
    m_WinManager.AddWindow( L"IDM_HIST_WINDOW", IDM_HIST_WINDOW, m_EvoHistWindow,  FALSE, FALSE ); 
    m_WinManager.AddWindow( L"IDM_DISP_WINDOW", IDM_DISP_WINDOW, m_DspOptWindow,   TRUE,  FALSE );
    m_WinManager.AddWindow( L"IDM_EDIT_WINDOW", IDM_EDIT_WINDOW, m_EditorWindow,   TRUE,  FALSE );
    m_WinManager.AddWindow( L"IDM_MINI_WINDOW", IDM_MINI_WINDOW, m_MiniGridWindow, TRUE,  FALSE );
    m_WinManager.AddWindow( L"IDM_MAIN_WINDOW", IDM_MAIN_WINDOW, m_MainGridWindow, TRUE,  FALSE );

	m_AppMenu.Initialize( m_hwndApp, & m_WorkThreadInterface, & m_WinManager );

	m_MiniGridWindow.Size( );
	adjustChildWindows( ); 

	if ( ! m_WinManager.GetWindowConfiguration( ) )
	{
		std::wcout << L"Using default window positions" << std::endl;
		Show( TRUE );
	}

	m_StatusBar.ClearStatusLine( );
	m_StatusBar.Show( TRUE );
	m_EditorWindow.Show( TRUE );
	m_MainGridWindow.Show( TRUE );

	(void)m_MainGridWindow.SendMessage( WM_COMMAND, IDM_FIT_ZOOM, 0 );
//	Script::ProcessScript( L"std_script.in" );

	m_bStopped = FALSE;
}

void AppWindow::Stop()
{
	m_bStopped = TRUE;

	m_MiniGridWindow     .Stop( );
	m_MainGridWindow     .Stop( );
	m_EvoHistWindow      .Stop( );
	m_EditorWindow       .Stop( );
	m_DspOptWindow       .Stop( );
	m_AppMenu            .Stop( );
	m_HistInfoWindow     .Stop( );
	m_Delay              .Stop( );
	m_ReadBuffer         .Stop( );
	m_WorkThreadInterface.Stop( );
	m_EvoHistGlue        .Stop( );  // deletes m_pModelDataWork

	m_WinManager.RemoveWindow( IDM_HIST_WINDOW ); 
	m_WinManager.RemoveWindow( IDM_DISP_WINDOW );
	m_WinManager.RemoveWindow( IDM_EDIT_WINDOW );
	m_WinManager.RemoveWindow( IDM_MINI_WINDOW );
	m_WinManager.RemoveWindow( IDM_MAIN_WINDOW );

	m_StatusBar.Show ( FALSE );
	m_Statistics.Show( FALSE );
	m_PerfWindow.Show( FALSE );
	m_CrsrWindow.Show( FALSE );

	delete m_pHistorySystem;   
	delete m_pEvoCore4Display; 

	m_pModelDataWork   = nullptr;
	m_pGraphics		   = nullptr;
	m_pHistorySystem   = nullptr;
	m_pEvoCore4Display = nullptr;
}

AppWindow::~AppWindow( )
{
	m_WorkThreadInterface.TerminateThread( );
}

LRESULT AppWindow::UserProc
( 
    UINT   const message, 
    WPARAM const wParam, 
    LPARAM const lParam 
)
{
    switch ( message )
    {

	case WM_ENTERMENULOOP:
		if ( wParam == FALSE )
			m_AppMenu.AdjustVisibility( );
		break;

	case WM_COMMAND:
    {
        int const wmId = LOWORD(wParam);
        switch (wmId)
        {
		case IDM_ABOUT:
            ShowAboutBox( GetWindowHandle( ) );
			break;

		case IDM_RESET:
		{
			int     iRes    = ResetDialog::Show( m_hwndApp );
			HCURSOR crsrOld = SetCursor( m_hCrsrWait );
			switch ( iRes )
			{
			case IDM_SOFT_RESET:
			case IDM_HISTORY_RESET:
				m_EvoController.ProcessCommand( iRes );
				break;
			case IDM_HARD_RESET:
				Stop();
				GridDimensions::DefineGridSize
				( 
					GRID_COORD( ResetDialog::GetNewWidth() ), 
					GRID_COORD( ResetDialog::GetNewHeight() ), 
					ResetDialog::GetNewNrOfNeighbors()
				);
				Start();
			}
			SetCursor( crsrOld );
		}
			break;

		case IDM_EXIT:
			PostMessage( WM_CLOSE, 0, 0 );
			break;

		default:
			m_EvoController.ProcessCommand( wParam, lParam );
            break;
        }
    }
    return FALSE;

    case WM_SIZE:
	case WM_MOVE:
		adjustChildWindows( );
		break;

	case WM_PAINT:
	{
		static COLORREF const CLR_GREY = RGB( 128, 128, 128 );
		PAINTSTRUCT   ps;
		HDC           hDC = BeginPaint( &ps );
		FillBackground( hDC, CLR_GREY );
		(void)EndPaint( &ps );
		return FALSE;
	}

	case WM_CLOSE:
		if ( ! m_bStopped )
			m_WinManager.StoreWindowConfiguration( );
		DestroyWindow( );        
		return TRUE;  

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        break;
    }
    
    return DefWindowProc( message, wParam, lParam );
}

void AppWindow::adjustChildWindows( )
{
    static PIXEL const HIST_WINDOW_HEIGHT = 30_PIXEL;

    PixelRectSize pntAppClientSize( GetClRectSize( ) );
	PIXEL pixAppClientWinWidth  = pntAppClientSize.GetX();
	PIXEL pixAppClientWinHeight = pntAppClientSize.GetY();

    if ( pntAppClientSize.IsNotZero( ) )
    {
        m_StatusBar.Resize( );
        pixAppClientWinHeight -= m_StatusBar.GetHeight( );
		pixAppClientWinHeight -= HIST_WINDOW_HEIGHT, 
        m_EvoHistWindow.Move   // adapt history window to new size
		( 
			0_PIXEL, 
			pixAppClientWinHeight, 
			pixAppClientWinWidth, 
			HIST_WINDOW_HEIGHT, 
			TRUE 
		); 
        m_MainGridWindow.Move
		( 
			0_PIXEL, 
			0_PIXEL, 
			pixAppClientWinWidth, 
			pixAppClientWinHeight, 
			TRUE 
		);
    }
}

//int MyAllocHook
//(
//	int allocType, 
//	void * userData, 
//	size_t size,
//	int blockType, 
//	long requestNumber,
//	const unsigned char* filename, 
//	int lineNumber
//)
//{
//	int x;
//
//	switch (allocType)
//	{
//	case _HOOK_ALLOC:
//		x = 1;
//		break;
//
//	case _HOOK_REALLOC:
//		x = 2;
//		break;
//
//	case _HOOK_FREE:
//		x = 3;
//		break;
//
//	default:
//		break;
//	}
//
//	return TRUE;
//}
//
