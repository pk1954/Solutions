// win32_parameterDlg.h
//
// NNetWindows

#pragma once

#include "NNetParameters.h"
#include "ObserverProxy.h"
#include "win32_baseDialog.h"

class NNetWorkThreadInterface;

class ParameterDialog : public BaseDialog
{
public:
	ParameterDialog( NNetWorkThreadInterface * const );
	~ParameterDialog( );

	void Start( HWND const, Param * const );
	void Stop( );

private:
	static DWORD const STYLE { WS_CHILD | WS_VISIBLE };
	static int   const HORZ_SPACE {  8 };
	static int   const VERT_SPACE { 16 };
	static int   const HEIGHT     { 16 };

	Param                   * m_pParams;
	NNetWorkThreadInterface * m_pNNetWorkThreadInterface;

	HWND m_hwndPeakVoltage;
	HWND m_hwndThreshold;       
	HWND m_hwndPulseWidth;      
	HWND m_hwndRefractoryPeriod;
	HWND m_hwndTimeResolution;
	HWND m_hwndPulseSpeed; 
		
	HWND createButton     ( HWND const, wchar_t const * const, int const, int const, int const, int const, HMENU const );
	HWND createEditField  ( HWND const, tParameter      const, int &,     int const, int const );
	HWND createStaticField( HWND const, wchar_t const * const, int &,     int const, int const );
	void resetParameter   ( HWND const, tParameter const );
	void applyParameter   ( HWND const, tParameter const );
	HWND addParameter     ( HWND const, tParameter const, int & );
	void applyParameters();
	void resetParameters();

	ParameterDialog             ( ParameterDialog const & );  // noncopyable class 
	ParameterDialog & operator= ( ParameterDialog const & );  // noncopyable class 

	virtual LRESULT UserProc( UINT const, WPARAM const, LPARAM const );
};