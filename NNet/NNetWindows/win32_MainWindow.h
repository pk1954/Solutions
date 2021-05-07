// win32_MainWindow.h : 
//
// NNetWindows

#pragma once

#include "Resource.h"
#include "scale.h"
#include "BaseKnot.h"
#include "MicroMeterPointVector.h"
#include "NNetModelReaderInterface.h"
#include "win32_animation.h"
#include "win32_NNetWindow.h"

class WinCommands;
class NNetModelCommands;
class ConnAnimationCommand;

using std::unique_ptr;

class MainWindow : public NNetWindow
{
public:

	void Start
	(
		HWND                     const,
		DWORD                    const,
		bool                     const,
		fPixel                   const,
		NNetModelReaderInterface const &,
		MonitorWindow            const &, 
		NNetController                 &,
		NNetModelCommands              &,
		WinCommands                    &,
		Observable                     &,
		Observable                     &
	);

	void Stop();
	void Reset();

	virtual long AddContextMenuEntries( HMENU const );

	NobId const GetTargetNobId     () const { return m_nobTarget; }
	NobId const GetHighlightedNobId() const { return m_nobHighlighted; }

	virtual bool Zoom( MicroMeter const, PixelPoint const * const );

	void ZoomStep( bool const, PixelPoint const * const );

	template <typename T>
	void NNetMove( T const & delta )	
	{ 
		GetDrawContext().Move( delta ); 
		Notify( true );  
		if ( m_pCoordObservable )
			m_pCoordObservable->NotifyAll( false );
	}

	MicroMeterPoint const GetCursorPos() const;

	void       CenterModel();
	void       CenterSelection();
	void       ShowArrows( bool const );
	bool const ArrowsVisible() const; 

	virtual bool OnCommand           ( WPARAM const, LPARAM const, PixelPoint const );
	virtual bool OnSize              ( WPARAM const, LPARAM const );
	virtual bool OnRButtonDown       ( WPARAM const, LPARAM const );
	virtual void OnMouseWheel        ( WPARAM const, LPARAM const );
	virtual void OnMouseMove         ( WPARAM const, LPARAM const );
	virtual void OnLeftButtonDblClick( WPARAM const, LPARAM const );
	virtual void OnLButtonUp         ( WPARAM const, LPARAM const );
	virtual bool OnRButtonUp         ( WPARAM const, LPARAM const );
	virtual void OnChar              ( WPARAM const, LPARAM const );
	virtual void OnPaint             ();

private:
	 
	inline static MicroMeter const STD_ARROW_SIZE { 30.0_MicroMeter };
	
	Scale m_scale;

	unique_ptr<Animation<MicroMeter>> m_upArrowAnimation;
	MicroMeter                        m_arrowSizeTarget { STD_ARROW_SIZE };
	MicroMeter                        m_arrowSize       { m_arrowSizeTarget };

	unique_ptr<Animation<PixelCoordsFp>> m_upCoordAnimation;

	MicroMeterRect      m_rectSelection        { };
	NobId             m_nobHighlighted     { };
	NobId             m_nobTarget          { };
	bool                m_bTargetFits          { false };
	Observable        * m_pCoordObservable     { nullptr };
	Observable        * m_pCursorPosObservable { nullptr };
	NNetModelCommands * m_pModelCommands       { nullptr };
	WinCommands       * m_pWinCommands         { nullptr };

	void setNoTarget        ();
	void setTargetNob     ();
	void setHighlightedNob( MicroMeterPoint const & );
	bool changePulseRate    ( NobId const, bool const );
	void centerAndZoomRect  ( UPNobList::SelMode const, float const );

	virtual void doPaint();

	virtual bool UserProc( UINT const, WPARAM const, LPARAM const );
};
