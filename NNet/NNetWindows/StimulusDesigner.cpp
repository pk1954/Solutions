// StimulusDesigner.cpp
//
// NNetWindows

#include <math.h>       // pow
#include "stdafx.h"
#include "PointType.h"
#include "NNetModelReaderInterface.h"
#include "SignalGenerator.h"
#include "NNetParameters.h"
#include "StimulusDesigner.h"

void StimulusDesigner::Start
(
	HWND                     const   hwndParent,
	SignalGenerator        * const   pSignalGenerator,
	NNetModelReaderInterface const & nmri
)
{
	HWND hwnd = StartBaseWindow
	(
		hwndParent,
		CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
		L"ClassMonitorWindow",
		WS_POPUPWINDOW | WS_CLIPSIBLINGS | WS_CAPTION | WS_SIZEBOX,
		nullptr,
		nullptr
	);
	m_graphics.Initialize(hwnd);
	SetWindowText(hwnd, L"StimulusDesigner");
	m_trackStruct.hwndTrack = hwnd;

	m_horzCoord.SetPixelSize(10000.0_MicroSecs); 
	m_horzCoord.SetPixelSizeLimits(1000._MicroSecs, 1000000._MicroSecs); 
	m_horzCoord.SetZoomFactor(1.3f);
	m_horzCoord.RegisterObserver(this);

	m_horzScale.InitHorzScale(&m_horzCoord, &m_graphics, L"s", 1e6f);
	m_horzScale.SetOrientation(false);
	m_horzScale.Recalc();

	m_vertCoord.SetPixelSize(0.25_fHertz);
	m_vertCoord.SetPixelSizeLimits(0.001_fHertz, 10._fHertz); 
	m_vertCoord.SetZoomFactor(1.3f);
	m_vertCoord.RegisterObserver(this);

	m_vertScale.InitVertScale(& m_vertCoord, & m_graphics, L"Hz", 1e0f);	
	m_vertScale.SetOrientation(true);
	m_vertScale.Recalc();

	m_pSignalGenerator      = pSignalGenerator;
	m_trackStruct.hwndTrack = hwnd;
	m_pNMRI                 = &nmri;

	m_hCrsrNS  = LoadCursor(NULL, IDC_SIZENS);
	m_hCrsrAll = LoadCursor(NULL, IDC_SIZEALL);
}

void StimulusDesigner::Reset()
{
	m_trackStruct.hwndTrack = HWND(0);
	(void)TrackMouseEvent(& m_trackStruct);
}

void StimulusDesigner::Stop()
{
	Reset();
	m_graphics.ShutDown();
	DestroyWindow();
}

fPixel const StimulusDesigner::getPixX(fMicroSecs const time) const
{
	return fPixel(m_horzCoord.Transform2fPixelPos(time));
}

fPixel const StimulusDesigner::getPixY(fHertz const freq) const
{
	return fPixel(m_vertCoord.Transform2fPixelPos(-freq));
}

fPixelPoint const StimulusDesigner::getPixPnt(fMicroSecs const time, fHertz const freq) const
{
	return fPixelPoint(getPixX(time), getPixY(freq));
}

fPixelPoint const StimulusDesigner::getGraphPnt(fMicroSecs const time) const
{
	fHertz const fHertzAct { m_pSignalGenerator->GetFrequency(time) };
	return getPixPnt(time, fHertzAct);
}

fPixelPoint const StimulusDesigner::getIntegralPnt(fMicroSecs const time) const
{
	float const fY { m_pSignalGenerator->StimulusIntegral(time) };
	return getPixPnt(time, fHertz(fY));
}

void StimulusDesigner::doPaint() const
{
	D2D1::ColorF const color         { D2D1::ColorF::Black };
	fMicroSecs   const usResolution  { m_pNMRI->TimeResolution() };
	fMicroSecs   const usPixelSize   { m_horzCoord.GetPixelSize() };
	fMicroSecs   const usIncrement   { (usPixelSize > usResolution) ? usPixelSize : usResolution };
	fMicroSecs   const timeStart     { 0.0_MicroSecs };
	fMicroSecs   const timeEnd       { m_horzCoord.Transform2logUnitSize(m_fPixGraphWidth) };
	fPixelPoint        prevPoint     { getGraphPnt(timeStart) };

	for (fMicroSecs time = timeStart + usIncrement; time < timeEnd; time += usIncrement)
	{
		fPixelPoint const actPoint  { getGraphPnt(time) };
		fPixelPoint const stepPoint { actPoint.GetX(), prevPoint.GetY() };
		m_graphics.DrawLine(prevPoint, stepPoint, m_fPixLineWidth, color);
		m_graphics.DrawLine(stepPoint, actPoint,  m_fPixLineWidth, color);
		prevPoint = actPoint;
	}

	//prevPoint = getIntegralPnt(timeStart);

	//for (fMicroSecs time = timeStart + usIncrement; time < timeEnd; time += usIncrement)
	//{
	//	fPixelPoint const actPoint  { getIntegralPnt(time) };
	//	fPixelPoint const stepPoint { actPoint.GetX(), prevPoint.GetY() };
	//	m_graphics.DrawLine(prevPoint, stepPoint, m_fPixLineWidth, D2D1::ColorF::Red);
	//	m_graphics.DrawLine(stepPoint, actPoint,  m_fPixLineWidth, D2D1::ColorF::Red);
	//	prevPoint = actPoint;
	//}

	m_horzScale.Display();
	m_vertScale.Display();

	switch (m_trackMode)
	{
	case tTrackMode::MAX_PNT:
		displayDiamondAtMaximum(DIAMOND_SIZE * 1.5f, D2D1::ColorF::Red);
		break;
	case tTrackMode::MAX_FREQ:
		displayFreqMaxLine(m_fPixLineWidth + 2.f, D2D1::ColorF::Red);
		break;
	case tTrackMode::MAX_TIME:
		displayTimeMaxLine(m_fPixLineWidth + 2.f, D2D1::ColorF::Red);
		break;
	case tTrackMode::BASE_FREQ:
		displayBaseFrequency(timeStart, timeEnd, m_fPixLineWidth + 2.f, D2D1::ColorF::Red);
		break;
	default:
		break;
	}
	displayDiamondAtMaximum(DIAMOND_SIZE, D2D1::ColorF::Lime);
	displayFreqMaxLine(m_fPixLineWidth, D2D1::ColorF::Green);
	displayTimeMaxLine(m_fPixLineWidth, D2D1::ColorF::Green);
	displayBaseFrequency(timeStart, timeEnd, m_fPixLineWidth, D2D1::ColorF::Green);
}

void StimulusDesigner::displayBaseFrequency
(
	fMicroSecs   const timeStart, 
	fMicroSecs   const timeEnd,
	fPixel       const fPixWidth,
	D2D1::ColorF const col
) const
{		
	fPixel      const fPixBaseFreq(getPixY(m_pSignalGenerator->GetBaseFrequency()));
	fPixelPoint const fPixPntStart(getPixX(timeStart), fPixBaseFreq);
	fPixelPoint const fPixPointEnd(getPixX(timeEnd),   fPixBaseFreq);
	m_graphics.DrawLine(fPixPntStart, fPixPointEnd, fPixWidth, col);
}

void StimulusDesigner::displayFreqMaxLine(fPixel const fPixWidth, D2D1::ColorF const col) const
{
	fMicroSecs  const usPeakTime { m_pSignalGenerator->GetPeakTime() };
	fPixelPoint const fPixPntMax { getGraphPnt(usPeakTime) };
	m_graphics.DrawLine
	(
		fPixelPoint(fPixPntMax.GetX(),            fPixPntMax.GetY()), 
		fPixelPoint(m_horzCoord.GetPixelOffset(), fPixPntMax.GetY()), 
		fPixWidth, 
		col
	);
}
void StimulusDesigner::displayTimeMaxLine(fPixel const fPixWidth, D2D1::ColorF const col) const
{
	fMicroSecs  const usPeakTime { m_pSignalGenerator->GetPeakTime() };
	fPixelPoint const fPixPntMax { getGraphPnt(usPeakTime) };
	m_graphics.DrawLine
	(
		fPixelPoint(fPixPntMax.GetX(), fPixPntMax.GetY()), 
		fPixelPoint(fPixPntMax.GetX(), m_vertCoord.GetPixelOffset()), 
		fPixWidth, 
		col
	);
}

void StimulusDesigner::displayDiamondAtMaximum(fPixel const fPixSize, D2D1::ColorF const col) const
{
	fMicroSecs  const usPeakTime { m_pSignalGenerator->GetPeakTime() };
	fPixelPoint const fPixPntMax { getGraphPnt(usPeakTime) };
	m_graphics.FillDiamond(fPixPntMax, fPixSize, col);
}

void StimulusDesigner::OnPaint()
{
	if (IsWindowVisible())
	{
		PAINTSTRUCT ps;
		HDC const hDC = BeginPaint(&ps);
		if (m_graphics.StartFrame(hDC))
		{
			doPaint();
			m_graphics.EndFrame();
		}
		EndPaint(&ps);
	}
}

bool StimulusDesigner::OnSize(WPARAM const wParam, LPARAM const lParam)
{
	static const float BORDER { 0.1f };
	UINT width  = LOWORD(lParam);
	UINT height = HIWORD(lParam);
	m_graphics.Resize(width, height);
	fPixel fPixWinWidth  { Convert2fPixel(PIXEL(width )) };
	fPixel fPixWinHeight { Convert2fPixel(PIXEL(height)) };
	fPixel fPixXoffset = fPixWinWidth * BORDER;
	fPixel fPixYoffset = fPixWinHeight * (1.0f - BORDER);
	m_fPixGraphWidth = fPixWinWidth  * (1.0f - 2.0f * BORDER);
	m_horzCoord.SetOffset(fPixXoffset);
	m_vertCoord.SetOffset(fPixYoffset);
	m_horzScale.SetOrthoOffset(fPixYoffset);
	m_vertScale.SetOrthoOffset(fPixXoffset);
	m_horzScale.Recalc();
	m_vertScale.Recalc();
	Trigger();  // cause repaint
	return true;
}

bool StimulusDesigner::timeMaxLineSelected(fPixelPoint const & pos) const
{
	fHertz      const freqStart  { 0.0_fHertz };
	fPixelPoint const fPixPntMax { getGraphPnt(m_pSignalGenerator->GetPeakTime()) };
	return (Distance(pos.GetX(), fPixPntMax.GetX()) <= 10.0_fPixel)
		&& (getPixY(freqStart) >= pos.GetY())
		&& (pos.GetY() >= fPixPntMax.GetY());
}

bool StimulusDesigner::freqMaxLineSelected(fPixelPoint const & pos) const
{
	fMicroSecs  const timeStart  { 0.0_MicroSecs };
	fPixelPoint const fPixPntMax { getGraphPnt(m_pSignalGenerator->GetPeakTime()) };
	return (Distance(pos.GetY(), fPixPntMax.GetY()) <= 10.0_fPixel)
		&& (getPixX(timeStart) <= pos.GetX())
		&& (pos.GetX() <= fPixPntMax.GetX());
}

bool StimulusDesigner::baseLineSelected(fPixelPoint const & pos) const
{
	fMicroSecs const timeStart { 0.0_MicroSecs };
	fMicroSecs const timeEnd   { m_horzCoord.Transform2logUnitSize(m_fPixGraphWidth) };
	fPixel     const fPixFreq  { getPixY(m_pSignalGenerator->GetBaseFrequency()) };
	return (Distance(pos.GetY(), fPixFreq) <= 10.0_fPixel)
		&& (getPixX(timeStart) <= pos.GetX())
		&& (pos.GetX() <= getPixX(timeEnd));
}

void StimulusDesigner::OnMouseMove(WPARAM const wParam, LPARAM const lParam)
{
	PixelPoint  const pixCrsrPos  { GetCrsrPosFromLparam(lParam) };
	fPixelPoint const fPixCrsrPos { Convert2fPixelPoint(pixCrsrPos) };
	fMicroSecs  const usPeakTime  { m_pSignalGenerator->GetPeakTime() };
	fHertz      const freqMax     { m_pSignalGenerator->GetFrequency(usPeakTime) };

	if (wParam & MK_LBUTTON)
	{
		fMicroSecs uSecs {   m_horzCoord.Transform2logUnitPos(fPixCrsrPos.GetX()) };
		fHertz     freq  { - m_vertCoord.Transform2logUnitPos(fPixCrsrPos.GetY()) };
		switch (m_trackMode)
		{
		case tTrackMode::MAX_PNT:
			m_pSignalGenerator->SetStimulusMax(uSecs, freq);
			break;
		case tTrackMode::MAX_FREQ:
			m_pSignalGenerator->SetStimulusMax(usPeakTime, freq);
			break;
		case tTrackMode::MAX_TIME:
			m_pSignalGenerator->SetStimulusMax(uSecs, freqMax);
			break;
		case tTrackMode::BASE_FREQ:
			m_pSignalGenerator->SetBaseFrequency(freq);
			break;
		default:
			break;
		}
	}
	else  // left button not pressed: select
	{
		fPixelPoint const fPixPntMax { getPixPnt(usPeakTime, freqMax) };
		if (Distance(fPixCrsrPos, fPixPntMax) <= 20.0_fPixel) 
			m_trackMode = tTrackMode::MAX_PNT;
		else if (baseLineSelected(fPixCrsrPos))
			m_trackMode = tTrackMode::BASE_FREQ;
		else if (freqMaxLineSelected(fPixCrsrPos))
			m_trackMode = tTrackMode::MAX_FREQ;
		else if (timeMaxLineSelected(fPixCrsrPos))
			m_trackMode = tTrackMode::MAX_TIME;
		else
			m_trackMode = tTrackMode::NONE;
	}
	Trigger();   // cause repaint
	(void)TrackMouseEvent(& m_trackStruct);
}

void StimulusDesigner::OnMouseWheel(WPARAM const wParam, LPARAM const lParam)
{  
	static float const ZOOM_FACTOR { 1.3f };

	int  const iDelta     { GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA };
	bool const bShiftKey  { (wParam & MK_SHIFT) != 0 };
	bool const bDirection { iDelta > 0 };
	bool       bResult    { true };

	for (int iSteps = abs(iDelta); (iSteps > 0) && bResult; --iSteps)
	{
		bResult = bShiftKey 
			      ? m_horzCoord.Zoom(bDirection)
			      : m_vertCoord.Zoom(bDirection);
	}
	if (!bResult)
		MessageBeep(MB_ICONWARNING);
}
