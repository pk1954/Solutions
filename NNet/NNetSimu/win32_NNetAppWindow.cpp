// win32_NNetAppWindow.cpp
//
// NNetSimu

#include "stdafx.h"
#include <chrono>
#include "WinUser.h"

// Model interfaces

#include "MoreTypes.h"
#include "NNetModel.h"
#include "NNetModelStorage.h"

// interfaces of various windows

#include "win32_NNetWindow.h"
#include "win32_status.h"
#include "win32_crsrWindow.h"
#include "win32_performanceWindow.h"
#include "win32_parameterDlg.h"
#include "win32_simulationControl.h"
#include "win32_slowMotionControl.h"
#include "win32_zoomControl.h"
#include "SlowMotionDisplay.h"
#include "TimeDisplay.h"

// infrastructure

#include "util.h"
#include "ObserverInterface.h"

// scripting and tracing

#include "trace.h"
#include "script.h"
#include "NNetWrappers.h"
#include "UtilityWrappers.h"
#include "win32_stopwatch.h"
#include "win32_fatalError.h"

// system and resources

#include "Resource.h"

// application

#include "win32_NNetAppWindow.h"

using namespace std::literals::chrono_literals;
using std::wostringstream;

NNetAppWindow::NNetAppWindow( ) :
	m_pSimulationControl( nullptr ),
	m_pMainNNetWindow( nullptr ),
	m_pModelDataWork( nullptr ),
	m_pSlowMotionDisplay( nullptr ),
	m_pTimeDisplay( nullptr ),
	m_pCrsrWindow( nullptr ),
	m_pPerformanceWindow( nullptr ),
	m_pNNetModelStorage( nullptr ),
	m_pNNetController( nullptr ),
	m_pParameterDlg( nullptr )
{
	Stopwatch stopwatch;

	DefineNNetWrappers( & m_NNetWorkThreadInterface );

	BaseAppWindow::Initialize( & m_NNetWorkThreadInterface ),
		
	m_pNNetReadBuffer = new NNetReadBuffer( );
	m_pPerformanceWindow = new PerformanceWindow( );

	NNetWindow::InitClass
	( 
		& m_NNetWorkThreadInterface, 
		& m_atDisplay 
	);

	m_pCursorPos      = new Observable();
	m_pAppMenu        = new NNetAppMenu( );
	m_pMainNNetWindow = new NNetWindow( );
	m_pCrsrWindow     = new CrsrWindow( );
	m_pParameterDlg   = new ParameterDialog( & m_NNetWorkThreadInterface );

	m_pMainNNetWindow   ->SetRefreshRate( 100ms );
	m_pCrsrWindow       ->SetRefreshRate( 100ms );
	m_pPerformanceWindow->SetRefreshRate( 500ms );
};

NNetAppWindow::~NNetAppWindow( )
{
	delete m_pMainNNetWindow;
	delete m_pAppMenu;
	delete m_pCrsrWindow;
	delete m_pParameterDlg;
	delete m_pPerformanceWindow;
}

void NNetAppWindow::Start( )
{
	BaseAppWindow::Start( m_pMainNNetWindow );
	m_pModelDataWork    = new NNetModel( );
	m_pNNetModelStorage = new NNetModelStorage( m_pModelDataWork );

	m_pAppMenu->Initialize
	( 
		m_hwndApp, 
		& m_NNetWorkThreadInterface, 
		& m_WinManager 
	);

	m_pNNetController = new NNetController
	( 
		m_pNNetModelStorage,
		m_pMainNNetWindow,
		& m_WinManager,
		& m_StatusBar, 
		& m_NNetWorkThreadInterface,
		& m_SlowMotionRatio
	);

	m_pMainNNetWindow->Start
	( 
		m_hwndApp, 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		nullptr,  // no visibility criterion. Allways visible,
		m_pModelDataWork,
		m_pCursorPos
	);

	m_pNNetReadBuffer->RegisterObserver( m_pMainNNetWindow );

//	m_pMainNNetWindow->ShowRefreshRateDlg( false );

	m_NNetWorkThreadInterface.Start
	( 
		m_hwndApp, 
		& m_atComputation,
		& m_eventPOI, 
		  m_pNNetReadBuffer,
		& m_SlowMotionRatio,
		m_pModelDataWork,
		TRUE    // async thread?
	);

	m_pCrsrWindow->Start( m_hwndApp, m_pCursorPos, m_pMainNNetWindow );
	m_pParameterDlg->Start( m_hwndApp, m_pModelDataWork );
	m_pPerformanceWindow->Start( m_hwndApp, & m_NNetWorkThreadInterface, & m_atDisplay );

	m_WinManager.AddWindow( L"IDM_CRSR_WINDOW",  IDM_CRSR_WINDOW,  * m_pCrsrWindow,        TRUE, FALSE );
	m_WinManager.AddWindow( L"IDM_MAIN_WINDOW",  IDM_MAIN_WINDOW,  * m_pMainNNetWindow,    TRUE, FALSE );
	m_WinManager.AddWindow( L"IDM_PARAM_WINDOW", IDM_PARAM_WINDOW, * m_pParameterDlg,      TRUE, FALSE );
	m_WinManager.AddWindow( L"IDM_PERF_WINDOW",  IDM_PERF_WINDOW,  * m_pPerformanceWindow, TRUE, FALSE );

	configureStatusBar( );

	if ( ! m_WinManager.GetWindowConfiguration( ) )
	{
		std::wcout << L"Using default window positions" << std::endl;
		Show( TRUE );
		m_pMainNNetWindow->Show( TRUE );
	}

	m_pCrsrWindow       ->Show( TRUE );
	m_pParameterDlg     ->Show( TRUE );
	m_pPerformanceWindow->Show( TRUE );

	PostCommand2Application( IDM_RUN, true );

	m_pNNetModelStorage->Write( std::wcout );
}

void NNetAppWindow::Stop()
{
	m_pMainNNetWindow->Stop( );
	m_pCrsrWindow    ->Stop( );
	m_pParameterDlg  ->Stop( );
	m_pPerformanceWindow->Stop( );

	m_pNNetReadBuffer->UnregisterAllObservers( );
	m_NNetWorkThreadInterface.Stop( );

	BaseAppWindow::Stop();

	delete m_pModelDataWork;
	delete m_pTimeDisplay;
	delete m_pSlowMotionDisplay;
	delete m_pNNetController;

	m_WinManager.RemoveAll( );
}

void NNetAppWindow::configureStatusBar( )
{
	int iPartScriptLine = 0;

	m_pTimeDisplay = new TimeDisplay( & m_StatusBar, m_pModelDataWork, iPartScriptLine );
	m_pNNetReadBuffer->RegisterObserver( m_pTimeDisplay );

	iPartScriptLine = m_StatusBar.NewPart( );
	m_pSimulationControl = new SimulationControl( & m_StatusBar, & m_NNetWorkThreadInterface );

	iPartScriptLine = m_StatusBar.NewPart( );
	m_pSlowMotionDisplay = new SlowMotionDisplay( & m_StatusBar, & m_SlowMotionRatio, iPartScriptLine );

	iPartScriptLine = m_StatusBar.NewPart( );
	SlowMotionControl::Add( & m_StatusBar );

	iPartScriptLine = m_StatusBar.NewPart( );
	m_ScriptHook.Initialize( & m_StatusBar, iPartScriptLine );
	m_StatusBar.DisplayInPart( iPartScriptLine, L"" );
	Script::ScrSetWrapHook( & m_ScriptHook );

	m_StatusBar.LastPart( );
	m_pTimeDisplay->Notify( true );
	m_pSlowMotionDisplay->Notify( true );
}

void NNetAppWindow::ProcessAppCommand( WPARAM const wParam, LPARAM const lParam )
{
	int const wmId = LOWORD( wParam );

	if ( wmId == IDM_FATAL_ERROR )
	{
		Stop();
		FatalError::Happened( static_cast<long>(lParam), L"unknown" );
	}

	try
	{
		if ( m_pNNetController->ProcessUIcommand( wmId, lParam ) )     // handle all commands that affect the UI
			return;                                                    // but do not concern the model
	}
	catch ( ... )
	{
		Stop();
		FatalError::Happened( 2, L"ProcessUIcommand" );
	}

	try
	{
		if ( m_pNNetController->ProcessModelCommand( wmId, lParam ) )
			ProcessFrameworkCommand( wmId, lParam );                   // Some commands are handled by the framework controller
	}
	catch ( ... )
	{
		Stop();
		FatalError::Happened( 3, L"ProcessModelCommand" );
	}
}