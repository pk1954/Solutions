// win32_actionTimer.h 
//
// win32_utilities

#pragma once

#include <chrono>
#include "util.h"
#include "NamedType.h"
#include "windows.h"
#include "win32_hiResTimer.h"
#include "ViewCollection.h"

using std::chrono::milliseconds;

using MilliHertz = NamedType< unsigned long, struct MilliHertz_Parameter >;

class ActionTimer
{
public:
	ActionTimer()
		: m_hrtimerSingleAction( ),
	      m_usSingleActionTime( 0 ),
		  m_hrtimerOverall( ),
		  m_dwCounter( 0 )
	{}

	void TimerStart( )
	{
		m_hrtimerSingleAction.Start( );
	};

	void TimerStop( )
	{
		m_hrtimerSingleAction.Stop( );
		m_usSingleActionTime = m_hrtimerSingleAction.GetDuration( );
		++m_dwCounter;
		m_observers.NotifyAll( false );
	};

	microseconds GetSingleActionTime( )
	{
		return m_usSingleActionTime;
	}

	MilliHertz CalcActionFrequency( DWORD dwCount, microseconds us )
	{
		if ( us == microseconds::zero() )
			return  MilliHertz(0);

		unsigned long long ullFrequency = ( dwCount * 1000000ull ) / us.count();
		return MilliHertz( CastToLong(ullFrequency) );
	}

	MilliHertz GetMeasuredPerformance( )
	{
		m_hrtimerOverall.Stop( );
		microseconds usOverallTime = m_hrtimerOverall.GetDuration( );
		MilliHertz result = CalcActionFrequency( m_dwCounter, usOverallTime ) * 1000;
		m_dwCounter = 0;
		m_hrtimerOverall.Start( );
		return result;
	}

	void RegisterObserver( ObserverInterface * const pObserver )
	{
		m_observers.Register( pObserver );
	}

	void Stop( )
	{
		m_observers.Clear();
	}

private:
	HiResTimer     m_hrtimerSingleAction;
	HiResTimer     m_hrtimerOverall;
	microseconds   m_usSingleActionTime; 
	DWORD          m_dwCounter;            // nr of executions
	ViewCollection m_observers;
};
