// TimeDisplay.cpp
//
// NNetSimu
//
// Displays time since start of simulation in status bar field

#include "stdafx.h"
#include <iostream>
#include <sstream> 
#include <string> 
#include "win32_status.h"
#include "win32_baseRefreshRate.h"
#include "NNetModelReaderInterface.h"
#include "TimeDisplay.h"

using std::to_wstring;
using std::wstring;
using std::wostringstream;

/////// inner class TimeDisplay::RefreshRate ///////

class TimeDisplay::RefreshRate : public BaseRefreshRate
{
public:
	RefreshRate	
	(
		StatusBar                      * pStatusBar,
		NNetModelReaderInterface const * pModelInterface,
		int                              iPartInStatusBar
	)
	:	m_pStatusBar           (pStatusBar),
		m_pNMRI(pModelInterface),
		m_iPartInStatusBar     (iPartInStatusBar)
	{ 
	}

	virtual void Trigger()
	{
		fMicroSecs const time = m_pNMRI->GetSimulationTime();
		wstring wstrTime;
		Format2wstring( time, wstrTime );
		m_pStatusBar->DisplayInPart( m_iPartInStatusBar, wstrTime );
	}

private:
	StatusBar                      * m_pStatusBar       { nullptr };
	NNetModelReaderInterface const * m_pNMRI            { nullptr };
	int                              m_iPartInStatusBar { -1 };
};

/////// functions of class TimeDisplay ///////

void TimeDisplay::Start
(
	StatusBar                      * pStatusBar,
	NNetModelReaderInterface const * pModelInterface,
	int                              iPartInStatusBar
)
{
	m_pRefreshRate = new RefreshRate
	( 
		pStatusBar,
		pModelInterface,
		iPartInStatusBar
	);
	m_pRefreshRate->SetRefreshRate( 300ms );
	pStatusBar->AddCustomControl( 8 );        // nr of characters 
}

void TimeDisplay::Stop()
{
	delete m_pRefreshRate;
	m_pRefreshRate = nullptr;
}

void TimeDisplay::Notify( bool const bImmediately )
{
	if ( m_pRefreshRate )
		m_pRefreshRate->Notify( bImmediately );
}
