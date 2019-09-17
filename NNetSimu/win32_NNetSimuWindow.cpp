// win32_NNetSimuWindow.cpp
//
// NNetSimu

#include "stdafx.h"
#include <chrono>
#include "WinUser.h"

using namespace std::literals::chrono_literals;

// Model interfaces

#include "NNetModel.h"

// interfaces of various windows

#include "win32_NNetWindow.h"
#include "win32_histWindow.h"

// infrastructure

#include "util.h"
#include "ObserverInterface.h"

// scripting and tracing

#include "trace.h"
#include "UtilityWrappers.h"
#include "win32_stopwatch.h"

// system and resources

#include "resource.h"

// application

#include "win32_NNetSimuWindow.h"

NNetSimuWindow::NNetSimuWindow( ) :
    BaseWindow( ),
	m_hwndConsole( nullptr ),
	m_pMainNNetWindow( nullptr ),
	m_pHistWindow( nullptr ),
	m_traceStream( ),
	m_bStarted( FALSE )
{
	Stopwatch stopwatch;

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

    // create window objects

	stopwatch.Start();

	m_NNetWorkThreadInterface.Initialize( & m_traceStream ); 

	stopwatch.Stop( L"create window objects" );

	NNetWindow::InitClass( & m_NNetWorkThreadInterface, & m_atDisplay );

	m_pMainNNetWindow = new NNetWindow( );
	m_pHistWindow     = new HistWindow( );

	m_NNetSimuController.Initialize
	( 
		this,
		& m_NNetWorkThreadInterface,
		& m_WinManager,
		& m_Delay, 
		& m_AppMenu
	);

	m_pMainNNetWindow->SetRefreshRate( 100ms );
	m_pHistWindow    ->SetRefreshRate( 200ms ); 
};

NNetSimuWindow::~NNetSimuWindow( )
{
	delete m_pMainNNetWindow;
	delete m_pHistWindow;
}

void NNetSimuWindow::Start( )
{
	NNetModel * pModelWork;

	m_AppMenu.Start( );

/////////////////////

	m_pHistorySystem = HistorySystem::CreateHistorySystem( );  // deleted in Stop function

///	m_pHistInfoWindow->SetHistorySystem( m_pHistorySystem );

	m_pModelDataWork     = m_NNetHistGlue.Start( m_pHistorySystem, TRUE ); 
	pModelWork           = m_pModelDataWork->GetNNetModel();
	m_pNNetModel4Display = NNetModel::CreateCore( );

	m_NNetReadBuffer.Initialize( pModelWork, m_pNNetModel4Display );

	m_pMainNNetWindow->Start
	( 
		m_hwndApp, 
		WS_CHILD | WS_CLIPSIBLINGS, 
		[&]() { return true; }	
	);
		
	m_NNetWorkThreadInterface.Start
	( 
		m_hwndApp, 
		& m_atComputation,
		& m_event, 
		& m_Delay, 
		& m_NNetReadBuffer, 
		& m_NNetHistGlue
	);

	m_pHistWindow  ->Start( m_hwndApp, m_pHistorySystem, & m_NNetWorkThreadInterface );

	m_WinManager.AddWindow( L"IDM_APPL_WINDOW", IDM_APPL_WINDOW,   m_hwndApp,         TRUE,  TRUE  );
	m_WinManager.AddWindow( L"IDM_MAIN_WINDOW", IDM_MAIN_WINDOW, * m_pMainNNetWindow, TRUE,  FALSE );
	m_WinManager.AddWindow( L"IDM_HIST_WINDOW", IDM_HIST_WINDOW, * m_pHistWindow,                   FALSE, FALSE ); 

	m_AppMenu.Initialize( m_hwndApp, & m_WinManager );

	if ( ! m_WinManager.GetWindowConfiguration( ) )
	{
		std::wcout << L"Using default window positions" << std::endl;
		Show( TRUE );
	}

	m_pMainNNetWindow->Show( TRUE );

	m_bStarted = TRUE;
}

void NNetSimuWindow::Stop()
{
	m_bStarted = FALSE;

	m_pMainNNetWindow->Stop( );

	m_AppMenu.Stop( );
	m_Delay  .Stop( );

	m_WinManager.RemoveAll( );
}

LRESULT NNetSimuWindow::UserProc
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
		m_NNetSimuController.ProcessCommand( wParam, lParam );
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
		if ( m_bStarted )
		{
			m_WinManager.StoreWindowConfiguration( );
		    Stop( );
		}
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

void NNetSimuWindow::adjustChildWindows( )
{
    static PIXEL const HIST_WINDOW_HEIGHT = 30_PIXEL;

    PixelRectSize pntAppClientSize( GetClRectSize( ) );
	PIXEL pixAppClientWinWidth  = pntAppClientSize.GetX();
	PIXEL pixAppClientWinHeight = pntAppClientSize.GetY();

    if ( pntAppClientSize.IsNotZero( ) )
    {
		//m_pStatusBar->Resize( );
		//pixAppClientWinHeight -= m_pStatusBar->GetHeight( );
		pixAppClientWinHeight -= HIST_WINDOW_HEIGHT, 
		m_pHistWindow->Move   // adapt history window to new size
		( 
			0_PIXEL, 
			pixAppClientWinHeight, 
			pixAppClientWinWidth, 
			HIST_WINDOW_HEIGHT, 
			TRUE 
		); 
		m_pMainNNetWindow->Move
		( 
			0_PIXEL, 
			0_PIXEL, 
			pixAppClientWinWidth, 
			pixAppClientWinHeight, 
			TRUE 
		);
	}
}