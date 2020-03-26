// win32_baseAppWindow.h
//
// NNetWindows

#pragma once

#include <fstream>
#include <iostream>
#include "DisplayFunctor.h"
#include "win32_baseWindow.h"
#include "win32_winManager.h"
#include "win32_status.h"

class NNetWorkThreadInterface;
class ModelWindow;
class AppMenu;

using std::wofstream;

class BaseAppWindow : public BaseWindow
{
public:
	virtual ~BaseAppWindow() {}; 

	void Initialize( NNetWorkThreadInterface * const );

	void Start( ModelWindow * const );

	virtual bool ProcessAppCommand( WPARAM const, LPARAM const = 0 ) = 0;
	virtual void ProcessCloseMessage( ) = 0;
	virtual void Stop( );

protected:
	HWND       m_hwndApp  { nullptr };
	AppMenu  * m_pAppMenu { nullptr };        // allocated by application
	WinManager m_WinManager { };
	StatusBar  m_StatusBar  { };

    bool ProcessFrameworkCommand( WPARAM const, LPARAM const = 0 );

private:

	HWND m_hwndConsole { nullptr };

	ModelWindow             * m_pModelWindow         { nullptr };
	NNetWorkThreadInterface * m_pWorkThreadInterface { nullptr };

	wofstream m_traceStream {};

	void adjustChildWindows( );

	virtual LRESULT UserProc( UINT const, WPARAM const, LPARAM const );
};	

class StatusBarDisplayFunctor : public DisplayFunctor
{
public:
	StatusBarDisplayFunctor( StatusBar * const pStatusBar, int const iPart )
	  : m_wstrBuffer( ),
		m_pStatusBar(pStatusBar),
		m_iPart( iPart)
	{ }

	virtual void operator() ( wstring const & line )
	{
		m_wstrBuffer = line;
		m_pStatusBar->DisplayInPart( m_iPart, m_wstrBuffer );
	}

private:
	wstring     m_wstrBuffer;
	StatusBar * m_pStatusBar;
	int         m_iPart;
};