// win32_delay.h 
//
// win32_utilities

#pragma once

#include "windows.h"
#include "ViewCollection.h"

class Delay
{
public:

	Delay()
	  : m_dwGenerationDelay( 0 )
	{}

	DWORD GetDelay( )
	{
		return m_dwGenerationDelay;
	};

	BOOL IsMaxSpeed( ) const
	{
		return m_dwGenerationDelay == 0;
	};

	void SetDelay( DWORD dwDelay )
	{
		m_dwGenerationDelay = dwDelay;
		m_observers.NotifyAll( FALSE );
	};

	void SleepDelay( ) const
	{
		if ( m_dwGenerationDelay > 0 )
			Sleep( m_dwGenerationDelay );
	};

	void RegisterObserver( ObserverInterface * const pObserver )
	{
		m_observers.Register( pObserver );
	}

	void Stop( )
	{
		m_observers.Clear();
	}

private:

	DWORD          m_dwGenerationDelay;  // in milliseconds
	ViewCollection m_observers;
};