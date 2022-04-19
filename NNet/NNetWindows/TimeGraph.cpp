// TimeGraph.cpp 
//
// NNetModel

#include "stdafx.h"
#include "TimeGraph.h"

TimeGraph::TimeGraph
(
	HWND                   const hwndParent,
	PixFpDimension<fMicroSecs> * pHorzCoord
)
  : m_pHorzCoord(pHorzCoord)
{
	GraphicsWindow::Initialize
	(
		hwndParent, 
		L"ClassTimeGraph", 
		WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE
	);
	assert(m_pHorzCoord);
	m_pHorzCoord->RegisterObserver(*this); 
};

TimeGraph::~TimeGraph()
{
	m_pHorzCoord->UnregisterObserver(*this); 
}

void TimeGraph::SetModelInterface(NNetModelWriterInterface * const p)
{
	assert(p);
	if (m_pNMWI)
		GetParams()->UnregisterObserver(*this);
	m_pNMWI = p;
	GetParams()->RegisterObserver(*this);
}

void TimeGraph::RegisterAtSigGen(SigGenId const id)
{
	if (SignalGenerator * pSigGen { m_pNMWI->GetSigGen(id) })
		pSigGen->Register(*this);
}

void TimeGraph::UnregisterAtSigGen(SigGenId const id)
{
	if (SignalGenerator * pSigGen { m_pNMWI->GetSigGen(id) })
		pSigGen->Unregister(*this);
}

fMicroSecs TimeGraph::getTime(fPixelPoint const & p) const 
{ 
	return m_pHorzCoord->Transform2logUnitPos(p.GetX()); 
}

fPixel TimeGraph::xTime(fMicroSecs const time) const 
{ 
	return fPixel(m_pHorzCoord->Transform2fPixelPos(time)); 
}

bool TimeGraph::OnSize(PIXEL const width, PIXEL const height)
{
	GraphicsWindow::OnSize(width, height);
	m_fPixRight  = Convert2fPixel(width);
	m_fPixBottom = Convert2fPixel(height);
	return true;
}