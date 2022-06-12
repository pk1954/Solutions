// MonitorControl.h
//
// NNetWindows

#pragma once

#include "Direct2D.h"
#include "MoreTypes.h"
#include "PixelTypes.h"
#include "PixFpDimension.h"
#include "Measurement.h"
#include "Track.h"
#include "TrackNr.h"
#include "SoundInterface.h"
#include "TimeGraph.h"

class MonitorData;
class NNetModelCommands;
class NNetModelWriterInterface;

struct IDWriteTextFormat;

class MonitorControl : public TimeGraph
{
public:
	MonitorControl
	(
		HWND const, 
		Sound                      &,
		NNetModelCommands          &,
		PixFpDimension<fMicroSecs> &,
		PixFpDimension<mV>         &
	);

	~MonitorControl() final = default;

	void Notify(bool const) final;

	void   SetModelInterface(NNetModelWriterInterface * const) final;
	LPARAM AddContextMenuEntries(HMENU const) final;

	void   Stop () final;
	void   Reset();
	bool   SignalTooHigh() const;
	float  ScaleFactor() const;
	void   ScaleSignals();
	void   StimulusTriggered();
	fPixel GetMaxSignal() const { return m_fPixMaxSignal; }

private:

	bool OnCommand        (WPARAM const, LPARAM const, PixelPoint const = PixelPoint::NULL_VAL()) final;
	void OnMouseLeave     () final;
	bool OnShow           (WPARAM const, LPARAM const) final;
	void OnMouseMove      (WPARAM const, LPARAM const) final;
	bool OnLButtonUp      (WPARAM const, LPARAM const) final;
	void OnLButtonDblClick(WPARAM const, LPARAM const) final;
	void OnChar           (WPARAM const, LPARAM const) final { /* */ };
	void OnMouseWheel     (WPARAM const, LPARAM const) final;
	bool OnSize           (PIXEL  const, PIXEL  const) final;

	void        moveOperation   (PixelPoint const &);
	void        selectSignal    (PixelPoint const &);
	void        selectTrack     (PixelPoint const &);
	SignalNr    findSignal      (TrackNr const, PixelPoint const &) const;
	TrackNr     findTrack       (PIXEL const) const;
	TrackNr     findPos4NewTrack(PIXEL const) const;
	void        highlightSignal (SignalId const &);
	fPixel      getSignalOffset (SignalId const &) const;
	fPixel      getSignalValue  (Signal const &, fMicroSecs const) const;
	fPixel      getfPixXpos     (fMicroSecs const) const;
	fPixelPoint getSignalPoint  (Signal const &, fMicroSecs const, fPixel const) const;
	fPixel      calcTrackHeight () const;
	fPixelPoint calcDiamondPos  () const;

	void paintWarningRect() const;
	void paintStimulusMarkers() const;
	void paintSignal(SignalId const &);
	void paintTrack(TrackNr const) const;
	void paintNumber(fPixel const, fPixel const, int const, D2D1::ColorF const) const;

	void DoPaint() final;

	inline static HCURSOR m_hCrsrWE { nullptr };
	inline static HCURSOR m_hCrsrNS { nullptr };

	PixFpDimension<fMicroSecs> & m_horzCoord;
	PixFpDimension<mV>         & m_vertCoord;
	Sound                      & m_sound;        
	NNetModelCommands          & m_modelCommands;
	MonitorData                * m_pMonitorData { nullptr };
	IDWriteTextFormat          * m_pTextFormat  { nullptr };
	Measurement                  m_measurement;

	TrackNr    m_trackNrHighlighted { TrackNr::NULL_VAL() };
	PixelPoint m_pixLast            { PP_NULL };     // last cursor position during selection 
	PIXEL      m_pixMoveOffsetY     { 0_PIXEL };     // vertical offset when moving signal
	fPixel     m_fPixWinWidth       { 0.0_fPixel };
	fPixel     m_fPixZeroX          { 0.0_fPixel };
	fPixel     m_fPixMaxSignal      { 0.0_fPixel };
};