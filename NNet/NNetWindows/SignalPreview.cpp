// SignalPreview.cpp 
//
// NNetWindows

module;

#include "Windows.h"
#include "SigGenDynamicData.h"
#include "SignalGenerator.h"

module SignalPreview;

import PixFpDimension;
import VoltageType;
import TimeGraph;
import BaseWindow;

SignalPreview::SignalPreview
(
	BaseWindow           const & baseWinParent,
	PixFpDimension<fMicroSecs> & horzCoord,
	PixFpDimension<mV>         & vertCoord
)
  : TimeGraph(baseWinParent.GetWindowHandle(), &horzCoord),
	m_vertCoord(vertCoord)
{
	m_vertCoord.RegisterObserver(*this);
};

SignalPreview::~SignalPreview()
{
	m_vertCoord.UnregisterObserver(*this);
}

void SignalPreview::DoPaint()
{
	if (SignalGenerator * const pSigGen { GetSigGenSelected() })
	if (Param     const * const pParams { GetParams() })
	{
		SigGenStaticData const & statData     { pSigGen->GetStaticData() };
		SigGenDynamicData        dynData      { };

		m_upGraphics->FillRectangle(Convert2fPixelRect(GetClPixelRect()), D2D1::ColorF::Ivory);

		dynData.StartStimulus();
		PaintCurve
		(
			[this, &dynData, &statData, pParams](fMicroSecs const stimuTime)
			{
				mV const voltage { dynData.SetTime(statData, *pParams, stimuTime) };
				return pixPntVolt(stimuTime, voltage); 
			}, 
			0.0_MicroSecs,
			getTime(xRight()),
			STD_WIDTH,
			D2D1::ColorF::Black
		);
	}
}

bool SignalPreview::OnSize(PIXEL const width, PIXEL const height)
{
	TimeGraph::OnSize(width, height);
	Trigger();  // cause repaint
	return true;
}

fPixel SignalPreview::getY(fPixel const fPix) const 
{ 
	return Convert2fPixel(GetClientWindowHeight()) - fPix; 
}

fPixel SignalPreview::yVolt(mV const volt) const 
{ 
	return getY(m_vertCoord.Transform2fPixelPos(volt)); 
}

fPixelPoint SignalPreview::pixPntVolt(fMicroSecs const t, mV const v) const 
{ 
	return fPixelPoint(xTime(t), yVolt(v)); 
}
