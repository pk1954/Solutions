// win32_NNetWindow.h : 
//
// NNetWindows

#pragma once

#include "MoreTypes.h"
#include "ShapeId.h"
#include "Direct2D.h"
#include "PixelCoordsFp.h"
#include "tHighlightType.h"
#include "SmoothMoveFp.h"
#include "win32_modelWindow.h"

using std::wstring;
using std::function;

class Scale;
class Observable;
class ActionTimer;
class AnimationThread;
class NNetWorkThreadInterface;

class NNetWindow : public ModelWindow
{
public:
	static void InitClass
	( 
		NNetWorkThreadInterface * const,
		ActionTimer             * const
	);

	NNetWindow( );

	void Start
	( 
		HWND             const, 
		DWORD            const,
		function<bool()> const,
		NNetModel      * const,
		Observable     * const
	);

	void Stop( );

	~NNetWindow( );

	ShapeId        const GetHighlightedShapeId( )          const { return m_shapeHighlighted; }
	ShapeId        const GetSuperHighlightedShapeId( )     const { return m_shapeSuperHighlighted; }
	tHighlightType const GetHighlightType( Shape const & ) const;

	void ResetHighlightedShape( ) { m_shapeHighlighted = NO_SHAPE; }
	void Zoom( bool const );
	void AnalysisFinished( );
	void ZoomKeepCrsrPos( MicroMeter const );
	void CenterModel( );

	void SelectShape  ( tBoolOp const );
	void SelectAll    ( tBoolOp const );
	void SelectSubtree( tBoolOp const );

	MicroMeter  GetPixelSize  ( ) const { return m_coord.GetPixelSize  (); }
	fPixelPoint GetPixelOffset( ) const { return m_coord.GetPixelOffset(); }
	void        SetPixelSize  ( MicroMeter  const s ) { m_coord.SetPixelSize  ( s ); }
	void        SetPixelOffset( fPixelPoint const f ) { m_coord.SetPixelOffset( f ); }

	MicroMeterPoint PixelPoint2MicroMeterPoint( PixelPoint const ) const;

	void PulseRateDlg   ( ShapeId const );
	void TriggerSoundDlg( ShapeId const );
	bool ChangePulseRate( ShapeId const, bool const );
	void ShowDirectionArrows( bool const );

private:
	NNetWindow             ( NNetWindow const & );  // noncopyable class 
	NNetWindow & operator= ( NNetWindow const & );  // noncopyable class 

	inline static NNetWorkThreadInterface * m_pNNetWorkThreadInterface { nullptr };
	inline static NNetModel               * m_pModel                   { nullptr };

	HMENU m_hPopupMenu { nullptr };
	BOOL  m_bMoveAllowed { TRUE };    // TRUE: move with mouse is possible

	D2D_driver        m_D2d_driver {};
	PixelCoordsFp     m_coord      {};
	Scale           * m_pScale               { nullptr };
	AnimationThread * m_pAnimationThread     { nullptr };
	Observable      * m_pCursorPosObservable { nullptr };

	PixelPoint m_ptLast            { PP_NULL };	// Last cursor position during selection 
	PixelPoint m_ptCommandPosition { PP_NULL };

	bool m_bFocusMode { false };

	MicroMeterPoint m_umPntCenterStart { MicroMeterPoint::NULL_VAL() }; // SmoothMove TODO: move these variables to SmootMoveFp
	MicroMeterPoint m_umPntCenterDelta { MicroMeterPoint::NULL_VAL() }; // SmoothMove 
	MicroMeter      m_umPixelSizeStart { MicroMeter::NULL_VAL() };      // SmoothMove 
	MicroMeter      m_umPixelSizeDelta { MicroMeter::NULL_VAL() };      // SmoothMove 
	SmoothMoveFp    m_smoothMove { };                                   // SmoothMove   

	MicroMeterRect m_umRectSelection { };

	ShapeId m_shapeHighlighted      { NO_SHAPE };
	ShapeId m_shapeSuperHighlighted { NO_SHAPE };

	virtual void AddContextMenuEntries( HMENU const, PixelPoint const );

	virtual void OnLeftButtonDblClick( WPARAM const, LPARAM const );
	virtual void OnMouseWheel        ( WPARAM const, LPARAM const );
	virtual void OnMouseMove         ( WPARAM const, LPARAM const );
	virtual BOOL OnCommand           ( WPARAM const, LPARAM const );
	virtual void OnLButtonUp         ( WPARAM const, LPARAM const );
	virtual bool OnRButtonUp         ( WPARAM const, LPARAM const );
	virtual void OnSetCursor         ( WPARAM const, LPARAM const );
	virtual void OnSize              ( WPARAM const, LPARAM const );
	virtual void OnLButtonDown       ( WPARAM const, LPARAM const );
	virtual bool OnRButtonDown       ( WPARAM const, LPARAM const );
	virtual void OnPaint( );

	void   smoothStep( );
	void   setStdFontSize( );
	LPARAM crsPos2LPARAM( ) const;
	void   centerAndZoomRect( MicroMeterRect const, float const );
	LPARAM pixelPoint2LPARAM( PixelPoint const ) const;
	BOOL   inObservedClientRect( LPARAM const );
	void   doPaint( );
};
