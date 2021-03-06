// win32_EvoEditor.h 
//
// EvoWindows

#pragma once

#include "EvolutionTypes.h"
#include "EvolutionCore.h"
#include "EvoReadBuffer.h"
#include "win32_baseDialog.h"

class EvoWorkThreadInterface;
class DspOptWindow;
class StatusBar;

class EvoEditorWindow : public BaseDialog
{
public:
    EvoEditorWindow( );
    ~EvoEditorWindow( );

    void Start
	( 
		HWND const, 
		EvoWorkThreadInterface * const, 
		EvoReadBuffer          * const, 
		DspOptWindow           * const
	);

	void Stop( );

	void    UpdateEditControls( );
	LRESULT SendClick( int ) const;


private:
    virtual LRESULT UserProc( UINT const, WPARAM const, LPARAM const );

	void setBrushMode       ( WORD const ) const;
	void setBrushShape      ( WORD const ) const;
	void setBrushManipulator( WORD const ) const;

	void updateOperationButtons( tBrushMode const ) const;

	EvoReadBuffer          * m_pReadBuffer;
    EvoWorkThreadInterface * m_pWorkThreadInterface;
    DspOptWindow           * m_pDspOptWindow;
};
