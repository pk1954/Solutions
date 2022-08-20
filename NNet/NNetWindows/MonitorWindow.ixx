// MonitorWindow.ixx
//
// NNetWindows

module;

#include <Windows.h>
#include "MonitorData.h"
#include "NNetModelWriterInterface.h"

export module MonitorWindow;

import BasicTypes;
import MoreTypes;
import VoltageType;
import PixelTypes;
import PixFpDimension;
import BaseWindow;
import MonitorControl;
import SoundInterface;
import NNetModelCommands;
import Scale;

export class MonitorWindow : public BaseWindow
{
public:

	MonitorWindow();
	~MonitorWindow() final;

	void Start(HWND const, Sound&, NNetModelCommands&);
	void Stop();
	void SetModelInterface(NNetModelWriterInterface* const) const;
	void StimulusTriggered() const;
	void ResetHorzCoord();

	PixFpDimension<fMicroSecs> const& HorzCoord() const { return m_horzCoord; }
	PixFpDimension<mV>         const& VertCoord() const { return m_vertCoord; }

private:
	inline static PIXEL const RIGHT_BORDER{ 25_PIXEL };
	inline static PIXEL const H_SCALE_HEIGHT{ 30_PIXEL };

	inline static fMicroSecs const DEFAULT_PIXEL_SIZE{ 100.0_MicroSecs };

	bool OnCommand(WPARAM const, LPARAM const, PixelPoint const) final;
	void OnPaint()                          final;
	bool OnSize(PIXEL  const, PIXEL const) final;

	wstring GetCaption() const final;

	PixFpDimension<fMicroSecs>    m_horzCoord;
	PixFpDimension<mV>            m_vertCoord;
	unique_ptr<Scale<fMicroSecs>> m_upHorzScale;
	unique_ptr<MonitorControl>    m_upMonitorControl;
};