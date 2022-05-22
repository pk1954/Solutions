// #include "Neuron.h"
//
// NNetWindows

#pragma once

#include "Direct2D.h"
#include "MoreTypes.h"
#include "PixFpDimension.h"
#include "NNetParameters.h"
#include "SignalGenerator.h"
#include "NNetModelWriterInterface.h"
#include "win32_graphicsWindow.h"

class TimeGraph : public GraphicsWindow
{
public:
	TimeGraph
	(
		HWND                   const hwndParent,
		PixFpDimension<fMicroSecs> * pHorzCoord
	);

	~TimeGraph();

	void SetModelInterface(NNetModelWriterInterface * const);

protected:

	fPixel const STD_WIDTH  { 1.0_fPixel };
	fPixel const HIGH_WIDTH { 3.0_fPixel };

	fPixel m_fPixRight  { 0.0_fPixel };
	fPixel m_fPixBottom { 0.0_fPixel };
	fPixel m_fPixLeft   { 0.0_fPixel };

	PixFpDimension<fMicroSecs> * m_pHorzCoord { nullptr };
	NNetModelWriterInterface   * m_pNMWI      { nullptr };

	SignalGenerator  const * GetSigGenSelected  () const { return m_pNMWI->GetSigGenSelected(); }
	SignalGenerator        * GetSigGenSelected  ()       { return m_pNMWI->GetSigGenSelected(); }
	SigGenStaticData const * GetSigGenStaticData() const { return & GetSigGenSelected()->GetStaticData(); }
	Param                  * GetParams          ()       { return & m_pNMWI->GetParams(); }

	fPixel PaintCurve
	(
		auto               getPoint,
		fMicroSecs   const usIncrement,
		fPixel       const fPixWidth,
		D2D1::ColorF const color          
	) const
	{
		fMicroSecs const timeStart     { 0.0_MicroSecs };
		fMicroSecs const usMax         { getTime(m_fPixRight) };
		fMicroSecs const timeEnd       { usMax };
		fPixelPoint      prevPoint     { getPoint(timeStart) };
		fPixel           fPixMaxSignal { 0._fPixel };

		for (fMicroSecs time = timeStart + usIncrement; time < timeEnd; time += usIncrement)
		{
			fPixelPoint const actPoint   { getPoint(time) };
			fPixel      const fPixSignal { actPoint.GetY() };
			fPixelPoint const stepPoint  { actPoint.GetX(), prevPoint.GetY() };
			if (fPixSignal > fPixMaxSignal)
				fPixMaxSignal = fPixSignal;
			if (prevPoint != stepPoint)
				m_upGraphics->DrawLine(prevPoint, stepPoint, fPixWidth, color);
			if (stepPoint != actPoint)
				m_upGraphics->DrawLine(stepPoint, actPoint,  fPixWidth, color);
			prevPoint = actPoint;
		}

		return fPixMaxSignal;
	}

	bool OnSize(PIXEL const, PIXEL const) override;

	fMicroSecs getTime(fPixelPoint const &) const;
	fPixel     xTime  (fMicroSecs  const  ) const;

	fPixel xLeft  () const { return m_fPixLeft;   }
	fPixel xRight () const { return m_fPixRight;  }
	fPixel yBottom() const { return m_fPixBottom; }
};