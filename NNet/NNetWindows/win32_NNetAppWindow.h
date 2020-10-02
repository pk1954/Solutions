// win32_NNetAppWindow.h : 
//
// NNetSimu

#pragma once

#include <fstream>
#include <iostream>
#include "SCRIPT.H"
#include "NNetModel.h"
#include "Preferences.h"
#include "SlowMotionRatio.h"
#include "ComputeThread.h"
#include "NNetModelStorage.h"
#include "CommandStack.h"
#include "DisplayFunctor.h"
#include "TimeDisplay.h"
#include "SlowMotionDisplay.h"
#include "UndoRedoMenu.h"
#include "BeaconAnimation.h"
#include "UnsavedChangesObserver.h"
#include "win32_event.h"
#include "win32_sound.h"
#include "win32_actionTimer.h"
#include "win32_winManager.h"
#include "win32_scriptHook.h"
#include "win32_MiniWindow.h"
#include "win32_MainWindow.h"
#include "win32_MonitorWindow.h"
#include "win32_NNetAppMenu.h"
#include "win32_NNetController.h"
#include "win32_parameterDlg.h"
#include "win32_crsrWindow.h"
#include "win32_modelDescription.h"
#include "win32_performanceWindow.h"
#include "win32_simulationControl.h"
#include "win32_slowMotionControl.h"
#include "win32_script.h"
#include "NNetModelReaderInterface.h"
#include "NNetModelWriterInterface.h"
#include "NNetModelCommands.h"
#include "NNetColors.h"

class ReadModelResult;
class MessagePump;
class Observable;
class NNetModel;

using std::wofstream;

class StatusBarDisplayFunctor : public DisplayFunctor
{
public:
	void Initialize( StatusBar * const pStatusBar, int const iPart )
	{
		m_pStatusBar = pStatusBar;
		m_iPart      = iPart;
	}

	virtual void operator() ( wstring const & line )
	{
		if ( m_pStatusBar )
		{
			m_wstring = line;
			m_pStatusBar->DisplayInPart( m_iPart, m_wstring );
		}
	}

private:
	wstring     m_wstring    { };
	StatusBar * m_pStatusBar { nullptr };
	int         m_iPart      { 0 };
};

class NNetAppWindow : public BaseWindow
{
public:
	NNetAppWindow( );
	~NNetAppWindow( );

	virtual void Start( MessagePump & );
	virtual void Stop( );

private:
	NNetAppWindow             ( NNetAppWindow const & );  // noncopyable class 
	NNetAppWindow & operator= ( NNetAppWindow const & );  // noncopyable class 

	virtual bool OnCommand( WPARAM const, LPARAM const, PixelPoint const );
	virtual bool OnSize   ( WPARAM const, LPARAM const );
	virtual void OnClose( );
	virtual void OnPaint( );

	void configureStatusBar( );
	void adjustChildWindows( );

	bool m_bStarted { false }; // if true, model is visible, all functions available

	HWND                      m_hwndConsole              { nullptr };
	HWND                      m_hwndApp                  { nullptr };
	ReadModelResult         * m_pReadModelResult         { nullptr };
	WinSound                  m_sound                    { };
	int                       m_statusMessagePart        { };
	wofstream                 m_traceStream              { };
	WinManager                m_WinManager               { };
	StatusBar                 m_StatusBar                { };
	Script                    m_script                   { };
	NNetAppMenu               m_appMenu                  { };
	ActionTimer               m_atComputation            { };
	ActionTimer               m_atDisplay                { };
	ScriptHook                m_ScriptHook               { };
	SlowMotionRatio           m_SlowMotionRatio          { };
	Observable                m_soundOnObservable        { };
	Observable                m_cursorPosObservable      { };
	Observable                m_blinkObservable          { };
	Observable                m_beaconObservable         { };
	Observable                m_modelTimeObservable      { };
	Observable                m_staticModelObservable    { };
	Observable                m_dynamicModelObservable   { };
	Observable                m_runObservable            { };
	Observable                m_performanceObservable    { };
	Observable                m_unsavedChangesObservable { };
	Observable                m_commandStackObservable   { };
	Observable                m_coordObservable          { };
	NNetModelReaderInterface  m_modelReaderInterface     { };
	NNetModelWriterInterface  m_modelWriterInterface     { };
	NNetModelCommands         m_modelCommands            { };
	ComputeThread             m_computeThread            { };
	DescriptionWindow         m_descWindow               { };
	CrsrWindow                m_crsrWindow               { };
	PerformanceWindow         m_performanceWindow        { };
	MainWindow                m_mainNNetWindow           { };
	MiniWindow                m_miniNNetWindow           { };
	MonitorWindow             m_monitorWindow            { };
	ParameterDialog           m_parameterDlg             { };
	Param                     m_parameters               { };
	NNetModelStorage          m_modelStorage             { };
	StatusBarDisplayFunctor   m_statusBarDispFunctor     { };
	NNetModel                 m_model                    { };
	NNetColors                m_NNetColors               { };
	NNetController            m_NNetController           { };
	SimulationControl         m_simulationControl        { };
	TimeDisplay               m_timeDisplay              { };
	SlowMotionDisplay         m_slowMotionDisplay        { };
	Preferences               m_preferences              { };
	CommandStack              m_cmdStack                 { };
	UndoRedoMenu              m_undoRedoMenu             { };
	UnsavedChangesObserver    m_unsavedChangesObserver   { };
	BeaconAnimation           m_beaconAnimation          { };
	ScriptFile                m_scriptFile               { };

	virtual bool UserProc( UINT const, WPARAM const, LPARAM const );
};
