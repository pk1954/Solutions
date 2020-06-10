// ComputeThread.h
//
// NNetSimu

#pragma once

#include "Observable.h"
#include "win32_event.h"
#include "win32_hiResTimer.h"
#include "win32_thread.h"
#include "NNetParameters.h"

class NNetModel;
class SlowMotionRatio;

class ComputeThread: public Util::Thread
{
public:

	ComputeThread();
	~ComputeThread();

	void Start
	(
		NNetModel       * const,
		Param           * const,
		SlowMotionRatio * const
	);

	virtual void ThreadStartupFunc( );
	virtual void ThreadMsgDispatcher( MSG const ) { };

	void Reset( );
	void SingleStep( ) { compute(); }

	void RunComputation( );
	void StopComputation( );
	bool IsRunning() const { return m_bContinue; }

	fMicroSecs GetTimeAvailPerCycle ( ) const { return m_usRealTimeAvailPerCycle; }
	fMicroSecs GetTimeSpentPerCycle ( ) const { return m_usRealTimeSpentPerCycle; }
	fMicroSecs GetRealTimeTilStart  ( ) const { return m_hrTimer.GetMicroSecsTilStart(); }
	fMicroSecs GetSimuTimeResolution( ) const { return m_pParam->GetTimeResolution(); }
	fMicroSecs GetSimulationTime    ( ) const;

	void AddRunObserver( ObserverInterface * pObserver )
	{
		m_runObservable.RegisterObserver( pObserver );
	}

	void AddPerformanceObserver( ObserverInterface * pObserver )
	{
		m_performanceObservable.RegisterObserver( pObserver );
	}

private:

	class TimeResObserver;

	long computeCyclesTodo( fMicroSecs const );
	void compute();

	TimeResObserver * m_pTimeResObserver        { nullptr };
	NNetModel       * m_pModel                  { nullptr };
	Param           * m_pParam                  { nullptr };
	SlowMotionRatio * m_pSlowMotionRatio        { nullptr };
	fMicroSecs        m_usRealTimeSpentPerCycle { 0.0_MicroSecs };
	fMicroSecs        m_usRealTimeAvailPerCycle { 0.0_MicroSecs };
	bool              m_bContinue               { false };
	bool              m_bWaiting                { false };
	HiResTimer        m_hrTimer                 { };
	Observable        m_runObservable           { };
	Observable        m_performanceObservable   { };
	Util::Event       m_runEvent                { };
};
