// win32_performanceWindow.cpp :
//

#include "stdafx.h"
#include "Strsafe.h"
#include "win32_performanceWindow.h"

PerformanceWindow::PerformanceWindow( ) : 
    TextWindow( ),
    m_hTimer( nullptr ),
    m_hrtimOverall( ),
    m_hrtimDisplay( ),
    m_hrtimCompute( ),
    m_dwModelTime( 0 ),
    m_dwDisplayTime( 0 ),
    m_dwGenerationDelay( 0 ),
    m_dwCounter( 0 )
{ }

void PerformanceWindow::Start( HWND const hwndParent, std::function<bool()> const visibilityCriterion )
{
    StartTextWindow
	( 
		hwndParent, 
		PixelRect { 0_PIXEL, 0_PIXEL, 300_PIXEL, 130_PIXEL }, 
		L"PerformanceWindow", 
		100,  // alpha
		TRUE,
		visibilityCriterion
	);
}

DWORD PerformanceWindow::calcFrequency( DWORD dwCount, DWORD dwMicroSecs )
{
    return dwMicroSecs ? ( ( dwCount * 1000 * 1000 ) / dwMicroSecs ) : 0;
}

DWORD PerformanceWindow::getMeasuredPerformance( )
{
    DWORD dwResult;
    DWORD dwMicroSecs;

    m_hrtimOverall.Stop( );
    dwMicroSecs = m_hrtimOverall.Get( );
    m_hrtimOverall.Start( );
    dwResult = calcFrequency( m_dwCounter, dwMicroSecs ) * 1000;
    m_dwCounter = 0;

    return dwResult;
}

void PerformanceWindow::printLine
(
	TextBuffer          & textBuf,
	wchar_t const * const pwch1, 
	DWORD           const dwValue, 
	wchar_t const * const pwch2 
)
{
    textBuf.printString       ( pwch1 );
    textBuf.printAsDecFraction( dwValue );
    textBuf.printString       ( pwch2 );
    textBuf.nextLine          ( );
}

void PerformanceWindow::DoPaint( TextBuffer & textBuf )
{
    DWORD const dwSumInMicroSeconds  = m_dwModelTime + m_dwGenerationDelay * 1000;
    DWORD const dwGensPer1000SecComp = calcFrequency( 1000, dwSumInMicroSeconds );
    DWORD const dwGensPer1000SecMeas = getMeasuredPerformance( );

    printLine( textBuf, L"Model:  ", m_dwModelTime,              L"ms"    );
    printLine( textBuf, L"Delay:  ", m_dwGenerationDelay * 1000, L"ms"    );
    printLine( textBuf, L"Comp:   ", dwGensPer1000SecComp,       L"Gen/s" );
    printLine( textBuf, L"Meas:   ", dwGensPer1000SecMeas,       L"Gen/s" );
    printLine( textBuf, L"Display:", m_dwDisplayTime,            L"ms"    );
}
