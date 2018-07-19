// win32_gridWindow.h : 
//

#pragma once

#include "win32_baseWindow.h"

class GridRect;
class DrawFrame;
class PixelCoordinates;
class PixelCore;
class WorkThreadInterface;
class DspOptWindow;
class EvolutionCore;
class EvolutionModelData;
class ObserverInterface;
class PerformanceWindow;
class EditorWindow;
class FocusPoint;

class GridWindow : public BaseWindow
{
public:
    GridWindow( );

    void Start
    ( 
        HWND, 
        WorkThreadInterface * const,
        EditorWindow        * const,
        FocusPoint          * const,
        DspOptWindow        * const,
        PerformanceWindow   * const, 
        EvolutionCore       * const, 
        EvolutionModelData  * const,
        DWORD                 const, 
        SHORT                 const
    );

    ~GridWindow( );

    void  Size( );
	short GetFieldSize( ) const;
	void  Zoom( bool const );
	void  ToggleStripMode( );
	void  ToggleClutMode ( );
	void  Escape( );
	void  SetZoom( SHORT const );
	void  Fit2Rect( );
	
	void Observe( GridWindow * const hgw )
	{
		m_pGWObserved  = hgw;
		m_bMoveAllowed = FALSE; 
	}

private:
    GridWindow             ( GridWindow const & );  // noncopyable class 
    GridWindow & operator= ( GridWindow const & );  // noncopyable class 
    
    WorkThreadInterface * m_pWorkThreadInterface;
    PixelCoordinates    * m_pPixelCoordinates;  // My own PixelCoordinates
    GridWindow          * m_pGWObserved;	 // Observed GridWindow (or nullptr)
    EvolutionModelData  * m_pModelWork;
    EditorWindow        * m_pEditorWindow;
    PerformanceWindow   * m_pPerformanceWindow;
    FocusPoint          * m_pFocusPoint;
    ObserverInterface   * m_pObserverInterface;
	PixelCore           * m_pPixelCore;
    DrawFrame           * m_pDrawFrame;
    PixelPoint 	          m_ptLast;	 	 // Last cursor position during selection 
    BOOL                  m_bMoveAllowed;  // TRUE: move with mouse is possible

    virtual LRESULT UserProc( UINT const, WPARAM const, LPARAM const );

	void mouseWheelAction( int );
    BOOL inObservedClientRect( LPARAM );
    void moveGrid( PixelPoint const & );
    void onMouseMove( LPARAM, WPARAM );
    void contextMenu( LPARAM );
    void doPaint( );
};
