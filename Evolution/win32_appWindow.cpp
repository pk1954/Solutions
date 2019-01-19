// win32_appWindow.cpp
//

#include "stdafx.h"

// EvolutionCore interfaces

#include "config.h"
#include "gridRect.h"
#include "EvolutionCoreWrappers.h"
#include "EvolutionCore.h"

// history system

#include "EvoHistorySysGlue.h"

// interfaces of various windows

#include "win32_gridWindow.h"
#include "win32_evoHistWindow.h"
#include "win32_editor.h"
#include "win32_displayOptions.h"
#include "win32_appWindow.h"
#include "win32_status.h"
#include "win32_statistics.h"
#include "win32_crsrWindow.h"
#include "win32_performanceWindow.h"
#include "win32_historyInfo.h"
#include "win32_aboutBox.h"

// infrastructure

#include "util.h"
#include "win32_winManager.h"
#include "win32_workThreadInterface.h"
#include "win32_focusPoint.h"
#include "win32_colorManager.h"
#include "ObserverInterface.h"

// scripting and tracing

#include "dump.h"
#include "trace.h"
#include "errhndl.h"
#include "script.h"
#include "UtilityWrappers.h"
#include "win32_stopwatch.h"
#include "win32_scriptHook.h"
#include "win32_wrappers.h"
#include "win32_editorWrappers.h"
#include "win32_histWrappers.h"

// system and resources

#include "d3d_system.h"
#include "Resource.h"

// application

#include "win32_evoController.h"
#include "win32_appWindow.h"

AppWindow::AppWindow( ) :
    BaseWindow( ),
    m_gridObservers( ),
    m_pMainGridWindow( nullptr ),
    m_pMiniGridWindow( nullptr ),
    m_pWorkThreadInterface( nullptr ),
    m_pPerfWindow( nullptr ),
    m_pEditorWindow( nullptr ),
    m_pCrsrWindow( nullptr ),
	m_pHistInfoWindow( nullptr ),
    m_pStatusBar( nullptr ),
    m_pStatistics( nullptr ),
    m_pDspOptWindow( nullptr ),
    m_pFocusPoint( nullptr ),
    m_pWinManager( nullptr ),
    m_pEvolutionCore( nullptr ),
    m_pScriptHook( nullptr ),
    m_pEvoHistWindow( nullptr ),
	m_pEvoHistGlue( nullptr ),
	m_pEvoController( nullptr ),
    m_traceStream( )
{};

void AppWindow::Start(  )
{
    HINSTANCE const hInstance = GetModuleHandle( nullptr );
    HWND      const hwndApp   = StartBaseWindow( nullptr, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,CS_HREDRAW | CS_VREDRAW, L"ClassAppWindow", WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN );

    SendMessage( WM_SETICON, ICON_BIG,   (LPARAM)LoadIcon( hInstance, MAKEINTRESOURCE( IDI_EVOLUTION ) ) );
    SendMessage( WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon( hInstance, MAKEINTRESOURCE( IDI_SMALL     ) ) );

    m_traceStream = OpenTraceFile( L"main_trace.out" );
	m_hwndConsole = Util::StdOutConsole( );

//	ScriptErrorHandler::ScrSetOutputStream( & wcout );
	
	DUMP::SetDumpStream( & wcout );

	Stopwatch stopwatch;

	stopwatch.Start();
	stopwatch.Stop( L"Stopwatch tara" );
	stopwatch.Start();
	Config::SetDefaultConfiguration( );
    Config::DefineConfigWrapperFunctions( );
	DefineUtilityWrapperFunctions( );
	stopwatch.Stop( L"setup std configuration" );

	stopwatch.Start();
	Script::ProcessScript( L"std_configuration.in" );
	stopwatch.Stop( L"process std_configuration.in" );

	stopwatch.Start();
	m_pEvolutionCore = EvolutionCore::InitClass( );
	m_pEvolutionCore->SetEvent( & m_event );
	stopwatch.Stop( L"EvolutionCore::InitClass" );

    D3dSystem::Create_D3D_Device( hwndApp, GRID_WIDTH.get(), GRID_HEIGHT.get(), Config::GetConfigValue( Config::tId::nrOfNeighbors ) == 6 );
	
    // create window objects

	stopwatch.Start();
    m_pFocusPoint          = new FocusPoint( );                          
    m_pWinManager          = new WinManager( );  
	m_pColorManager        = new ColorManager( );
    m_pStatusBar           = new StatusBar( );       
    m_pCrsrWindow          = new CrsrWindow( );   
	m_pHistInfoWindow      = new HistInfoWindow( );
    m_pDspOptWindow        = new DspOptWindow( );       
    m_pEditorWindow        = new EditorWindow( );    
    m_pStatistics          = new StatisticsWindow( );   
    m_pMainGridWindow      = new GridWindow( );   
    m_pMiniGridWindow      = new GridWindow( );   
    m_pPerfWindow          = new PerformanceWindow( );  
    m_pEvoHistWindow       = new EvoHistWindow( );
	m_pEvoController       = new EvoController( );
	m_pEvoHistGlue         = new EvoHistorySysGlue( );
	m_pWorkThreadInterface = new WorkThreadInterface( & m_traceStream );
	stopwatch.Stop( L"create window objects" );

	stopwatch.Start();
    m_pHistorySystem = HistorySystem::CreateHistorySystem( );
	stopwatch.Stop( L"CreateHistorySystem" );

	stopwatch.Start();
    SetMenu( hwndApp, LoadMenu( hInstance, MAKEINTRESOURCE( IDC_EVOLUTION_MAIN ) ) );
	Util::SetApplicationTitle( hwndApp, IDS_APP_TITLE );
	if ( Config::GetConfigValue( Config::tId::nrOfNeighbors ) == 6 )
        EnableMenuItem( GetMenu( hwndApp ), IDD_TOGGLE_STRIP_MODE, MF_GRAYED );  // strip mode looks ugly in heaxagon mode
    DefineWin32HistWrapperFunctions( m_pWorkThreadInterface );
	stopwatch.Stop( L"Application setup" );

	stopwatch.Start();
	m_pHistInfoWindow     ->Start( hwndApp, m_pHistorySystem );
	m_pEvoHistGlue        ->Start( m_pEvolutionCore, m_pHistorySystem, Util::GetMaxNrOfSlots( EvolutionCore::GetModelSize( ) ), true, m_pHistInfoWindow );
	m_pEvoHistWindow      ->Start( hwndApp, m_pFocusPoint, m_pEvoHistGlue, m_pWorkThreadInterface );
    m_pStatusBar          ->Start( hwndApp, m_pEvolutionCore );
	m_pFocusPoint         ->Start( m_pEvoHistGlue, m_pEvolutionCore );
	m_pWorkThreadInterface->Start( hwndApp, m_pColorManager, m_pPerfWindow, m_pEditorWindow, & m_event, & m_gridObservers, m_pEvolutionCore, m_pEvoHistGlue );
	m_pDspOptWindow       ->Start( hwndApp, m_pEvolutionCore );
    m_pEditorWindow       ->Start( hwndApp, m_pWorkThreadInterface, m_pEvolutionCore, m_pDspOptWindow, m_pStatusBar );
    m_pMainGridWindow     ->Start( hwndApp, m_pWorkThreadInterface, m_pFocusPoint, m_pDspOptWindow, m_pPerfWindow, m_pColorManager, m_pEvolutionCore, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 16 );
    m_pMiniGridWindow     ->Start( hwndApp, m_pWorkThreadInterface, m_pFocusPoint, m_pDspOptWindow, m_pPerfWindow, m_pColorManager, m_pEvolutionCore, WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CAPTION, 2 );
    m_pStatistics         ->Start( hwndApp, m_pEvolutionCore );
    m_pCrsrWindow         ->Start( hwndApp, m_pFocusPoint, m_pEvolutionCore );
    m_pPerfWindow         ->Start( hwndApp, 100 );
	m_pEvoController      ->Start( & m_traceStream, m_pWorkThreadInterface, m_pWinManager, m_pPerfWindow, m_pStatusBar, m_pMainGridWindow, m_pEditorWindow, m_pColorManager );
	stopwatch.Stop( L"Start windows" );

	stopwatch.Start();
	
    m_pStatusBar->SetRefreshRate     ( 300 );
    m_pEvoHistWindow->SetRefreshRate ( 200 ); 
    m_pCrsrWindow->SetRefreshRate    ( 100 );
    m_pStatistics->SetRefreshRate    ( 100 );
    m_pPerfWindow->SetRefreshRate    ( 100 );
	m_pHistInfoWindow->SetRefreshRate( 300 );
    m_pMiniGridWindow->SetRefreshRate( 300 );
    m_pMainGridWindow->SetRefreshRate( 100 );
	
    m_gridObservers.AttachObserver( m_pStatusBar      );
    m_gridObservers.AttachObserver( m_pEvoHistWindow  ); 
    m_gridObservers.AttachObserver( m_pCrsrWindow     );
    m_gridObservers.AttachObserver( m_pStatistics     );
    m_gridObservers.AttachObserver( m_pPerfWindow     );
    m_gridObservers.AttachObserver( m_pMiniGridWindow );
    m_gridObservers.AttachObserver( m_pMainGridWindow );
	
	m_pEvolutionCore->SetObservers( & m_gridObservers );   // display callback for core

    m_pWinManager->AddWindow( L"IDM_APPL_WINDOW", IDM_APPL_WINDOW, hwndApp,                              TRUE,  TRUE );
    m_pWinManager->AddWindow( L"IDM_CONS_WINDOW", IDM_CONS_WINDOW, m_hwndConsole,                        TRUE,  TRUE );
	m_pWinManager->AddWindow( L"IDM_STATUS_BAR",  IDM_STATUS_BAR,  m_pStatusBar     ->GetWindowHandle(), FALSE, FALSE );
    m_pWinManager->AddWindow( L"IDM_HIST_WINDOW", IDM_HIST_WINDOW, m_pEvoHistWindow ->GetWindowHandle(), FALSE, FALSE ); 
    m_pWinManager->AddWindow( L"IDM_DISP_WINDOW", IDM_DISP_WINDOW, m_pDspOptWindow  ->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_EDIT_WINDOW", IDM_EDIT_WINDOW, m_pEditorWindow  ->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_HIST_INFO",   IDM_HIST_INFO,   m_pHistInfoWindow->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_CRSR_WINDOW", IDM_CRSR_WINDOW, m_pCrsrWindow    ->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_STAT_WINDOW", IDM_STAT_WINDOW, m_pStatistics    ->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_PERF_WINDOW", IDM_PERF_WINDOW, m_pPerfWindow    ->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_MINI_WINDOW", IDM_MINI_WINDOW, m_pMiniGridWindow->GetWindowHandle(), TRUE, FALSE );
    m_pWinManager->AddWindow( L"IDM_MAIN_WINDOW", IDM_MAIN_WINDOW, m_pMainGridWindow->GetWindowHandle(), TRUE, FALSE );
	stopwatch.Stop( L"Window manager setup" );

	stopwatch.Start();
    m_pMiniGridWindow->Observe( m_pMainGridWindow );  // mini window observes main grid window
    m_pMiniGridWindow->Size( );
    m_pScriptHook = new ScriptHook( m_pStatusBar );
    Script::ScrSetWrapHook( m_pScriptHook );
    DefineWin32WrapperFunctions( m_pWorkThreadInterface );
    DefineWin32EditorWrapperFunctions( m_pEditorWindow );
	stopwatch.Stop( L"Other setup tasks" );

	stopwatch.Start();
    if ( ! m_pWinManager->GetWindowConfiguration( ) )
	{
		wcout << L"Using default window positions" << endl;
		Show( TRUE );
	}
	stopwatch.Stop( L"Get window configuration" );

	m_pStatusBar->ClearStatusLine( );

    (void)m_pMainGridWindow->SendMessage( WM_COMMAND, IDM_FIT_ZOOM, 0 );
	m_pEvoController->ProcessCommand( IDM_SET_SIMU_MODE, static_cast<LPARAM>(tBoolOp::opFalse) );
//	Script::ProcessScript( L"std_script.in" );
}

void AppWindow::shutDown()
{
    m_pWinManager->StoreWindowConfiguration( );
    m_pWorkThreadInterface->TerminateThread( );
	m_pStatistics->TerminateTextWindow();
    m_pPerfWindow->TerminateTextWindow();
    m_pCrsrWindow->TerminateTextWindow();
	m_pHistInfoWindow->TerminateTextWindow();
	DestroyWindow( GetWindowHandle( ) );        
}

AppWindow::~AppWindow( )
{
    try
    {
        if ( Config::UseHistorySystem( ) )
        {
            delete m_pEvoHistWindow;
			delete m_pEvoHistGlue;
        }

		delete m_pWorkThreadInterface;
		delete m_pStatistics;
		delete m_pPerfWindow;
		delete m_pCrsrWindow; 
		delete m_pEditorWindow;
		delete m_pDspOptWindow;
		delete m_pStatusBar;
		delete m_pHistInfoWindow;
		delete m_pMiniGridWindow;
		delete m_pMainGridWindow;
		delete m_pEvolutionCore;
        delete m_pFocusPoint;
        delete m_pWinManager;
        delete m_pScriptHook;
		delete m_pEvoController;
    }
    catch ( ... )
    {
        exit( 1 );
    };
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

    case WM_COMMAND:
    {
        int const wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            ShowAboutBox( GetWindowHandle( ) );
            break;

        case IDM_ADJUST_MINI_WIN:
			adjustMiniWinVisibility( static_cast<int>(lParam) );
            break;

        case IDM_EXIT:
            PostMessage( WM_CLOSE, 0, 0 );
            break;

        default:
			m_pEvoController->ProcessCommand( wParam, lParam );
            break;
        }
    }
    return FALSE;

    case WM_SIZE:
    case WM_MOVE:
        adjustChildWindows( );
        break;

    case WM_CLOSE:
		shutDown();
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
    static int const HIST_WINDOW_HEIGHT = 30;

    PixelRectSize pntAppClientSize( GetClRectSize( ) );

    if ( ! pntAppClientSize.IsEmpty( ) )
    {
        m_pStatusBar->Resize( );
        pntAppClientSize.ReduceHeight( m_pStatusBar->GetHeight( ) );

        if ( m_pEvoHistWindow != nullptr )
        {
            m_pEvoHistWindow->Move   // adapt history window to new size
			( 
				0, 
				pntAppClientSize.GetHeight( ) - HIST_WINDOW_HEIGHT, 
				pntAppClientSize.GetWidth(), 
				HIST_WINDOW_HEIGHT, 
				TRUE 
			); 
        }

        m_pMainGridWindow->Move( 0, 0, pntAppClientSize.GetWidth( ), pntAppClientSize.GetHeight( ), TRUE );
    }
}

void AppWindow::adjustMiniWinVisibility( int const iMode )
{
	Config::tOnOffAuto onOffAuto;
	if ( iMode != 0 )
	{
		switch ( iMode )
		{
			case IDM_MINI_WINDOW_ON:
				onOffAuto = Config::tOnOffAuto::on; 
				break;
			case IDM_MINI_WINDOW_OFF:
				onOffAuto = Config::tOnOffAuto::off; 
				break;
			case IDM_MINI_WINDOW_AUTO:
				onOffAuto = Config::tOnOffAuto::automatic; 
				break;
			default:
				assert( false );
				break;
		}
		Config::SetConfigValue( Config::tId::miniGridDisplay, static_cast<long>( onOffAuto ) ); 
	}
	else 
	{
		onOffAuto = Config::GetConfigValueOnOffAuto( Config::tId::miniGridDisplay ); 
	}

	switch ( onOffAuto )
	{
		case Config::tOnOffAuto::on:
			m_pMiniGridWindow->Show( true );
			break;

		case Config::tOnOffAuto::off:
			m_pMiniGridWindow->Show( false );
			break;

		case Config::tOnOffAuto::automatic:
			m_pMiniGridWindow->Show( ! m_pMainGridWindow->IsFullGridVisible( ) );
			break;
	}
}
