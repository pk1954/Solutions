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

#include "win32_NNetEditor.h"
#include "win32_NNetWindow.h"
#include "win32_status.h"
#include "win32_crsrWindow.h"
#include "win32_simulationControl.h"
#include "win32_slowMotionControl.h"
#include "win32_zoomControl.h"
#include "SlowMotionDisplay.h"
#include "TimeDisplay.h"

// infrastructure

#include "util.h"
#include "LogarithmicTrackBar.h"
#include "ObserverInterface.h"

// scripting and tracing

#include "trace.h"
#include "script.h"
#include "UtilityWrappers.h"
#include "win32_stopwatch.h"

// system and resources

#include "Resource.h"
#include "d3d_system.h"

// application

#include "win32_NNetAppWindow.h"

using namespace std::literals::chrono_literals;

NNetAppWindow::NNetAppWindow( ) :
	m_pMainNNetWindow( nullptr ),
	m_pModelDataWork( nullptr ),
	m_pGraphics( nullptr ),
	m_pSlowMotionDisplay( nullptr ),
	m_pTimeDisplay( nullptr ),
	m_pCrsrWindow( nullptr )
{
	Stopwatch stopwatch;

	BaseAppWindow::Initialize( & m_NNetWorkThreadInterface, FALSE ),
		
	m_pNNetReadBuffer = new NNetReadBuffer( FALSE );  // no double buffering

	NNetWindow::InitClass
	( 
		  m_pNNetReadBuffer,
		& m_NNetWorkThreadInterface, 
		& m_atDisplay 
	);

	m_pAppMenu          = new NNetAppMenu( );
	m_pMainNNetWindow   = new NNetWindow( );
	m_pNNetEditorWindow = new NNetEditorWindow( );
	m_pCrsrWindow       = new CrsrWindow( );

	m_pNNetController = new NNetController
	( 
		this,
		  m_pMainNNetWindow,
		  m_pNNetEditorWindow,
		& m_WinManager,
		& m_StatusBar, 
		& m_NNetWorkThreadInterface,
		& m_SlowMotionRatio
	);

	m_pMainNNetWindow  ->SetRefreshRate( 100ms );
	m_pNNetEditorWindow->SetRefreshRate( 300ms );
	m_pCrsrWindow      ->SetRefreshRate( 100ms );
};

NNetAppWindow::~NNetAppWindow( )
{
	delete m_pNNetController;
	delete m_pNNetEditorWindow;
	delete m_pMainNNetWindow;
	delete m_pAppMenu;
	delete m_pCrsrWindow;
}

void NNetAppWindow::Start( )
{
	m_D3d_driver.Initialize
	( 
		m_hwndApp, 
		200,   // TODO ????
		100, 
		FALSE 
	);

	m_pGraphics = & m_D3d_driver;

	Shape::SetGraphics( m_pGraphics );
	BaseAppWindow::Start( m_pMainNNetWindow );
	m_pAppMenu->Initialize
	( 
		m_hwndApp, 
		& m_NNetWorkThreadInterface, 
		& m_WinManager 
	);

	m_pModelDataWork    = new NNetModel( );
	m_pNNetModelStorage = new NNetModelStorage( m_pModelDataWork );

	m_pNNetReadBuffer->Initialize( m_pModelDataWork, m_pModelDataWork );

	m_pMainNNetWindow->Start
	( 
		m_hwndApp, 
		m_pGraphics, 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		nullptr  // no visibility criterion. Allways visible
	);
		
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

	m_pCrsrWindow->Start( m_hwndApp, m_pNNetReadBuffer, m_pMainNNetWindow );

	m_WinManager.AddWindow( L"IDM_CRSR_WINDOW", IDM_CRSR_WINDOW, * m_pCrsrWindow,     TRUE, FALSE );
	m_WinManager.AddWindow( L"IDM_MAIN_WINDOW", IDM_MAIN_WINDOW, * m_pMainNNetWindow, TRUE, FALSE );

	configureStatusBar( );

	if ( ! m_WinManager.GetWindowConfiguration( ) )
	{
		std::wcout << L"Using default window positions" << std::endl;
		Show( TRUE );
		m_pMainNNetWindow->Show( TRUE );
	}

	m_pCrsrWindow->Show( TRUE );

	PostCommand2Application( IDM_RUN, true );

	m_pNNetModelStorage->Write( std::wcout );
	m_pNNetModelStorage->Read( L"F:\\SW-projects\\Evolution\\NNetSimu\\test.nmod" );
}

void NNetAppWindow::Stop()
{
	m_pMainNNetWindow->Stop( );
	m_pCrsrWindow    ->Stop( );

	m_pNNetReadBuffer->UnregisterAllObservers( );
	m_NNetWorkThreadInterface.Stop( );

	BaseAppWindow::Stop();

	delete m_pModelDataWork;
	delete m_pTimeDisplay;
	delete m_pSlowMotionDisplay;

	m_WinManager.RemoveAll( );
}

void NNetAppWindow::configureStatusBar( )
{
	int iPartScriptLine = 0;

	//iPartScriptLine = m_StatusBar.NewPart( );
	//m_StatusBar.AddButton( L"Show editor", (HMENU)IDM_EDIT_WINDOW, BS_PUSHBUTTON );

	//ZoomControl::AddSizeControl
	//( 
	//	& m_StatusBar, 
	//	LogarithmicTrackbar::Value2TrackbarD( MINIMUM_PIXEL_SIZE.GetValue() ), 
	//	LogarithmicTrackbar::Value2TrackbarD( MAXIMUM_PIXEL_SIZE.GetValue() ), 
	//	LogarithmicTrackbar::Value2TrackbarD( DEFAULT_PIXEL_SIZE.GetValue() ) 
	//);
	//EnableWindow( m_StatusBar.GetDlgItem( IDM_FIT_ZOOM ), FALSE );

	//iPartScriptLine = m_StatusBar.NewPart( );
	m_pTimeDisplay = new TimeDisplay( & m_StatusBar, m_pNNetReadBuffer, iPartScriptLine );

	iPartScriptLine = m_StatusBar.NewPart( );
	SimulationControl::Add( & m_StatusBar );

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

	if ( m_pNNetController->ProcessUIcommand( wmId, lParam ) )     // handle all commands that affect the UI
		return;                                                    // but do not concern the model

	if ( m_pNNetController->ProcessModelCommand( wmId, lParam ) )
		ProcessFrameworkCommand( wmId, lParam );                   // Some commands are handled by the framework controller
}