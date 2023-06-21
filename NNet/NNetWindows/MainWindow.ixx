// MainWindow.ixx
//
// NNetWindows

module;

#include <Windows.h>
#include <memory>
#include "Resource.h"

export module NNetWin32:MainWindow;

import Types;
import ActionTimer;
import ThreadPoolTimer;
import NNetModel;
import NNetSignals;
import :Preferences;
import :SelectionMenu;
import :NNetWindow;

using std::unique_ptr;

export class MainWindow : public NNetWindow
{
public:

	void Start
	(
		HWND   const,
		bool   const,
		fPixel const,
		Preferences&,
		NNetController&,
		Observable&,
		Observable&,
		Observable&,
		ActionTimer* const,
		MonitorWindow const *
	);

	void Stop() final;
	void Reset();

	LPARAM AddContextMenuEntries(HMENU const) final;

	template <typename T>
	void NNetMove(T const& delta)
	{
		GetDrawContext().Move(delta);
		if (m_pCoordObservable)
			m_pCoordObservable->NotifyAll(false);
	}

	MicroMeterPnt GetCursorPos() const;

	NobId GetHighlightedNobId()         const { return m_nobIdHighlighted; }
	bool  IsHighlighted(NobId const id) const { return id == m_nobIdHighlighted; }
	bool  IsHighlighted(Nob const& nob) const { return IsHighlighted(nob.GetId()); }

	void CenterModel();
	void CenterSelection();

	void OnPaint()                                                       final;
	void OnChar           (WPARAM const, LPARAM const)                   final;
	bool OnCommand        (WPARAM const, LPARAM const, PixelPoint const) final;
	bool OnRButtonDown    (WPARAM const, LPARAM const)                   final;
	void OnMouseWheel     (WPARAM const, LPARAM const)                   final;
	void OnMouseMove      (WPARAM const, LPARAM const)                   final;
	void OnLButtonDblClick(WPARAM const, LPARAM const)                   final;
	bool OnLButtonUp      (WPARAM const, LPARAM const)                   final;
	bool OnLButtonDown    (WPARAM const, LPARAM const)                   final;
	bool OnSize           (PIXEL  const, PIXEL  const)                   final;
	bool OnMove           (PIXEL  const, PIXEL  const)                   final;

private:

	inline static PIXEL const H_SCALE_HEIGHT { 30_PIXEL };
	inline static PIXEL const V_SCALE_WIDTH  { 35_PIXEL };

	unique_ptr<Scale<MicroMeter>> m_upHorzScale { };
	unique_ptr<Scale<MicroMeter>> m_upVertScale { };

	MicroMeterPnt   m_umDelta                { NP_ZERO };
	fPixelPoint     m_fPixScaleSize          { fPP_ZERO };
	MicroMeter      m_umArrowSize            { 0._MicroMeter };
	Preferences*    m_pPreferences           { nullptr };
	ActionTimer*    m_pDisplayTimer          { nullptr };
	Observable*     m_pCoordObservable       { nullptr };
	Observable*     m_pCursorPosObservable   { nullptr };
	Observable*     m_pStaticModelObservable { nullptr };
	NobId           m_nobIdHighlighted       { NO_NOB };
	NobId           m_nobIdTarget            { NO_NOB };
	SigGenId        m_idSigGenUnderCrsr      { NO_SIGGEN };
	SensorId        m_sensorIdSelected       { SensorId::NULL_VAL() };
	SelectionMenu   m_SelectionMenu;

	unique_ptr<ThreadPoolTimer> m_upTimer;

	NobId    findTargetNob(MicroMeterPnt const&);
	bool     setHighlightedNob   (MicroMeterPnt const&);
	bool     setHighlightedSensor(MicroMeterPnt const&);
	void     selectSignalHandle  (MicroMeterPnt const&);
	void     centerAndZoomRect(UPNobList::SelMode const, float const);
	bool     connectionAllowed();
	void     select(NobId const);
	SigGenId getSigGenId(LPARAM const);
	bool     selectSigGen(SigGenId const);
	void     PaintGraphics() final;
	void     drawInputCable(InputLine const &) const;
	void     connect(NobId const, NobId const);
	void     adjustScales();
	fPixel   sigGenOffset() const
	{ 
		return m_pPreferences->ScalesVisible()
			? m_upVertScale->GetOrthoOffset()
			: 0._fPixel;
	}

	bool  UserProc(UINT const, WPARAM const, LPARAM const) final;
};
