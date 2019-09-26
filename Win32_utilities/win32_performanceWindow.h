// win32_performanceWindow.h 
//
// Toolbox/win32_utilities

#pragma once

#include "win32_delay.h"
#include "win32_textWindow.h"

class ActionTimer;

class PerformanceWindow: public TextWindow
{
public:
    PerformanceWindow( );
    ~PerformanceWindow( ) {}; 

    void Start
	( 
		HWND const, 
		Delay &, 
		ActionTimer &, 
		ActionTimer &,
		std::function<bool()> const 
	);

	void Stop( );

    virtual void DoPaint( TextBuffer & );

private:
	void printLine( TextBuffer &, wchar_t const * const, microseconds const );
	void printLine( TextBuffer &, wchar_t const * const, MilliHertz   const );

	ActionTimer * m_pAtComputation;
	ActionTimer * m_pAtDisplay;
	Delay       * m_pDelay;
};
