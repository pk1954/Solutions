// TimeGraph.ixx
//
// Win32_utilities

module;

#include <d2d1.h>
#include <cassert>
#include <compare>
#include <math.h>
#include <Windows.h>

export module TimeGraph;

import Types;
import SaveCast;
import Scale;
import PixFpDimension;
import Direct2D;
import GraphicsWindow;

export class TimeGraph : public GraphicsWindow
{
public:
	TimeGraph(HWND const hwndParent);

	~TimeGraph() override;

	void SetRightBorder(fPixel const b) { m_fPixRightBorder = b; }

	virtual void SetHorzCoord(PixFpDimension<fMicroSecs>*);

	PixFpDimension<fMicroSecs>       * GetHorzCoord ()       { return m_pHorzCoord; }
	PixFpDimension<fMicroSecs> const * GetHorzCoordC() const { return m_pHorzCoord; }

protected:

	fPixel Paint
	(
		auto                  getPoint,
		fMicroSecs      const timeStart0,
		fMicroSecs      const timeEnd,
		fMicroSecs      const usResolution,
		ID2D1SolidColorBrush* pBrush,
		fPixel          const fPixWidth
	) const
	{
		fMicroSecs const timeStart     { usResolution * Cast2Float(floor(timeStart0 / usResolution)) };
		fPixel           fPixMinSignal { fPixel::MAX_VAL() };
		fPixelPoint      prevPoint     { getPoint(timeStart) };
		if (prevPoint.IsNull())
			return fPixMinSignal;

		if (m_pHorzCoord->Transform2fPixelSize(usResolution) >= 3._fPixel)
		{
			for (fMicroSecs time = timeStart + usResolution; time < timeEnd; time += usResolution)
			{
				fPixelPoint const actPoint { getPoint(time) };
				if (actPoint.IsNull())
					break;
				if (actPoint.GetY() < fPixMinSignal)
					fPixMinSignal = actPoint.GetY();

				fPixelPoint const stepPoint { actPoint.GetX(), prevPoint.GetY() };
				m_upGraphics->DrawLine(prevPoint, stepPoint, fPixWidth, *pBrush);
				m_upGraphics->DrawLine(stepPoint, actPoint,  fPixWidth, *pBrush);
				prevPoint = actPoint;
			}
		}
		else
		{
			fPixel fPixYmin { prevPoint.GetY() };

			for (fMicroSecs time = timeStart + usResolution; time < timeEnd; time += usResolution)
			{
				fPixelPoint actPoint { getPoint(time) };
				assert(actPoint.IsNotNull());
				if (actPoint.GetY() < fPixYmin)
				{
					fPixYmin = actPoint.GetY();
					if (fPixYmin < fPixMinSignal)
						fPixMinSignal = fPixYmin;
				}
				if (actPoint.GetX() - prevPoint.GetX() > 1.0_fPixel)
				{
					actPoint.SetY(fPixYmin);
					m_upGraphics->DrawLine(prevPoint, actPoint, fPixWidth, *pBrush);
					prevPoint = actPoint;
					fPixYmin = fPixel::MAX_VAL();
				}
			}
		}
		return fPixMinSignal;
	}

	bool OnSize(PIXEL const, PIXEL const) override;

	virtual fMicroSecs GetTime(fPixel const) const;

	fMicroSecs GetTime(fPixelPoint const &) const;
	fPixel     xTime  (fMicroSecs  const  ) const;

	fPixel xLeft       () const { return m_fPixLeft;   }
	fPixel xRight      () const { return m_fPixRight - m_fPixRightBorder; }
	fPixel xRightBorder() const { return m_fPixRightBorder; }
	fPixel yBottom     () const { return m_fPixBottom; }

	fMicroSecs getMaxTime() const { return GetTime(xRight()); }

private:

	PixFpDimension<fMicroSecs> * m_pHorzCoord { nullptr };

	fPixel m_fPixRightBorder { 0.0_fPixel };
	fPixel m_fPixRight       { 0.0_fPixel };
	fPixel m_fPixBottom      { 0.0_fPixel };
	fPixel m_fPixLeft        { 0.0_fPixel };
};