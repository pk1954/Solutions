// win32_NNetAppWindow.h : 
//
// NNetSimu

#pragma once

#include <fstream>
#include <iostream>
#include "win32_event.h"
#include "win32_actionTimer.h"

// infrastructure

#include "win32_appWindowInterface.h"
#include "win32_winManager.h"
#include "win32_scriptHook.h"
#include "SlowMotionRatio.h"

// application

#include "win32_NNetWorkThreadInterface.h"
#include "win32_NNetWindow.h"
#include "win32_NNetAppMenu.h"
#include "win32_NNetReadBuffer.h"
#include "win32_NNetController.h"

class NNetModel;
class CrsrWindow;
class TimeDisplay;
class SlowMotionDisplay;
class NNetModelStorage;
class ParameterDialog;

class NNetAppWindow : public AppWindowInterface
{
public:
	NNetAppWindow( );
	~NNetAppWindow( );

	virtual void Start( );
	virtual void Stop( );

	void OpenModel( );

private:
	NNetAppWindow             ( NNetAppWindow const & );  // noncopyable class 
	NNetAppWindow & operator= ( NNetAppWindow const & );  // noncopyable class 

	virtual void ProcessAppCommand( WPARAM const, LPARAM const = 0 );

	void configureStatusBar( );

	Util::Event              m_eventPOI;

	NNetWorkThreadInterface m_NNetWorkThreadInterface;
	ActionTimer             m_atComputation;
	ActionTimer             m_atDisplay;
	ScriptHook              m_ScriptHook;
	SlowMotionRatio         m_SlowMotionRatio;

	NNetReadBuffer        * m_pNNetReadBuffer;
	NNetController        * m_pNNetController;
	NNetModel             * m_pModelDataWork;
	NNetWindow            * m_pMainNNetWindow;
	TimeDisplay           * m_pTimeDisplay;
	SlowMotionDisplay     * m_pSlowMotionDisplay;
	CrsrWindow            * m_pCrsrWindow;
	NNetModelStorage      * m_pNNetModelStorage;
	ParameterDialog       * m_pParameterDlg;
};
