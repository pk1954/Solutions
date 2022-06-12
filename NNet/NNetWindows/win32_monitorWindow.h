// win32_monitorWindow.h : 
//
// NNetWindows

#pragma once

#include "MoreTypes.h"
#include "PixFpDimension.h"
#include "win32_scale.h"
#include "win32_baseWindow.h"

class NNetModelWriterInterface;
class NNetModelCommands;
class MonitorControl;
class MonitorData;
class Sound;

class MonitorWindow : public BaseWindow
{
public:

	MonitorWindow();
	~MonitorWindow() final;

	void Start(HWND const, Sound &,	NNetModelCommands &);
	void Stop();
	void SetModelInterface(NNetModelWriterInterface * const);
	void StimulusTriggered();

	PixFpDimension<fMicroSecs> const & HorzCoord() const { return m_horzCoord; }
	PixFpDimension<mV>         const & VertCoord() const { return m_vertCoord; }

private:
	inline static PIXEL const RIGHT_BORDER   { 25_PIXEL };
	inline static PIXEL const H_SCALE_HEIGHT { 30_PIXEL };

	void OnPaint()                          final;
	bool OnSize (PIXEL  const, PIXEL const) final;

	wstring GetCaption() const final;

	PixFpDimension<fMicroSecs>    m_horzCoord;
	PixFpDimension<mV>            m_vertCoord;
	unique_ptr<Scale<fMicroSecs>> m_upHorzScale;
	unique_ptr<MonitorControl>    m_upMonitorControl;
};
