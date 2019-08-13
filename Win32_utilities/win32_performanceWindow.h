// win32_performanceWindow.h 
//

#pragma once

#include "win32_hiResTimer.h"
#include "win32_textWindow.h"

class PerformanceWindow: public TextWindow
{
public:
    PerformanceWindow( );
    ~PerformanceWindow( ) {}; 

    void Start( HWND const );

    void ComputationStart( )
    {
        m_hrtimCompute.Start( );
    };

    void ComputationStop( )
    {
        m_hrtimCompute.Stop( );
        m_dwModelTime = m_hrtimCompute.Get( );
        ++m_dwCounter;
    };

    void DisplayStart( )
    {
        m_hrtimDisplay.Start( );
    };

    void DisplayStop( )
    {
        m_hrtimDisplay.Stop( );
        m_dwDisplayTime = m_hrtimDisplay.Get( );
    };

    DWORD GetPerfGenerationDelay(  )
    {
        return m_dwGenerationDelay;
    };

	BOOL IsMaxSpeed( )
	{
		return m_dwGenerationDelay == 0;
	};

    void SetPerfGenerationDelay( DWORD dwDelay )
    {
        m_dwGenerationDelay = dwDelay;
        Invalidate( FALSE );   // Redraw, do not erase background
    };

    void SleepDelay( ) const
    {
		if ( m_dwGenerationDelay > 0 )
			Sleep( m_dwGenerationDelay );
    };

    virtual void DoPaint( TextBuffer & );

private:
    DWORD getMeasuredPerformance( );
	DWORD calcFrequency( DWORD, DWORD );
    void  printLine( TextBuffer &, wchar_t const * const, DWORD const, wchar_t const * const );

    HANDLE     m_hTimer;
    HiResTimer m_hrtimOverall;
    HiResTimer m_hrtimDisplay;
    HiResTimer m_hrtimCompute;
    DWORD      m_dwModelTime;        // in microseconds
    DWORD      m_dwDisplayTime;      // in milliseconds
    DWORD      m_dwGenerationDelay;  // in milliseconds
    DWORD      m_dwCounter;          // nr of model computations
};
