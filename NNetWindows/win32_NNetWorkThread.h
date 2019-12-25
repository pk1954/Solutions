// win32_NNetWorkThread.h
//
// NNetWindows

#pragma once

#include "win32_hiResTimer.h"
#include "win32_WorkThread.h"

class ActionTimer;
class RootWindow;
class WinManager;
class EventInterface;
class SlowMotionRatio;
class NNetModel;
class NNetHistorySysGlue;
class NNetWorkThreadInterface;

class NNetWorkThreadMessage
{
public:
	enum class Id : UINT
	{
		NNET_FIRST = WorkThreadMessage::FIRST_APP_MESSAGE,
		RESET_TIMER,
		RESET_MODEL,
		TRIGGER,
		CONNECT,
		REMOVE_SHAPE,
		DISCONNECT,
		PULSE_RATE,
		PULSE_SPEED,
		DAMPING_FACTOR,
	    THRESHOLD,
	    PEAK_VOLTAGE, 
	    PULSE_WIDTH,         
        REFRACTORY_PERIOD,
		MOVE_SHAPE,
		SLOW_MOTION_CHANGED,
		NEW_NEURON,
		NEW_INPUT_NEURON,
		APPEND_NEURON,
		APPEND_INPUT_NEURON,
		ADD_OUTGOING,
		ADD_INCOMING,
		INSERT_NEURON,
		NNET_LAST,
		FIRST = NNET_FIRST,
		LAST = NNET_LAST
	};

	static BOOL IsValid( NNetWorkThreadMessage::Id msg )
	{
		return (NNetWorkThreadMessage::Id::FIRST <= msg) && (msg <= NNetWorkThreadMessage::Id::LAST);
	}
};

class NNetWorkThread: public WorkThread
{
public:
	NNetWorkThread
	( 
		HWND                      const,
		ActionTimer             * const,
		EventInterface          * const,
		ObserverInterface       * const,
		SlowMotionRatio         * const,
		NNetWorkThreadInterface * const,
		NNetModel               * const,
		BOOL                      const
	);
	~NNetWorkThread( );

private:

	void ResetTimer()
	{
		m_hrTimer.Restart();
	}

	virtual void SetRunModeHook( BOOL const bState ) 
	{
		if ( bState )
			m_hrTimer.Start();
		else
			m_hrTimer.Stop();
	}

	virtual BOOL Dispatch( MSG const );

	virtual void WaitTilNextActivation( );

	virtual void Compute();

	NNetModel       * m_pNNetModel;
	SlowMotionRatio * m_pSlowMotionRatio;
	HiResTimer        m_hrTimer;
};
