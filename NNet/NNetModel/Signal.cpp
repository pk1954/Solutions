// Signal.cpp 
//
// NNetModel

#include "stdafx.h"
#include "util.h"
#include "DrawContext.h"
#include "SimulationTime.h"
#include "UPNobList.h"
#include "NNetColors.h"
#include "BaseKnot.h"
#include "SignalSource.h"
#include "Signal.h"

using std::wcout;
using std::endl;
using std::make_unique;

Signal::Signal
(
    Observable   & observable,
    SignalSource & sigSrc
) 
    : m_dynModelObservable(observable),
      m_sigSource(sigSrc)
//      m_timeStart(SimulationTime::Get())
{
    m_iSourceType = SIGSRC_CIRCLE;
    m_dynModelObservable.RegisterObserver(*this);
    m_sigSource         .RegisterObserver(*this);
}

Signal::~Signal()
{
    m_sigSource         .UnregisterObserver(*this);
    m_dynModelObservable.UnregisterObserver(*this);
}

void Signal::Reset(UPNobList const & list)    
{ 
    m_timeStart = 0.0_MicroSecs;
}

bool Signal::Includes(MicroMeterPnt const p) const 
{ 
    return m_sigSource.Includes(p); 
}

void Signal::WriteSignalInfo(wostream & out) const
{
    m_sigSource.WriteInfo(out);
}

void Signal::WriteSignalData(wostream & out) const
{
    size_t const iLast { m_data.size() - 1 };
    out << L" StartTime " << m_timeStart << endl;
    out << LIST_OPEN_BRACKET << m_data.size() << NR_SEPARATOR;
    for (size_t i = 0;; ++i)
    {
        if (i % 10 == 0)
            out << endl;
        out << m_data[i];
        if (i == iLast)
            break;
        out << ID_SEPARATOR;
    }
    out << endl << LIST_CLOSE_BRACKET << endl;
}

void Signal::Draw
(
    DrawContext const & context,
    bool        const   bHighlight
) const
{
    m_sigSource.Draw(context, bHighlight);
}

SIG_INDEX Signal::time2index
(
    Param      const & param,
    fMicroSecs         usParam
) const
{
    if (usParam < m_timeStart)
        return INVALID_SIG_INDEX;
    fMicroSecs const timeTilStart { usParam - m_timeStart };
    float      const fNrOfPoints  { timeTilStart / param.TimeResolution() };
    SIG_INDEX  const index        { static_cast<SIG_INDEX>(roundf(fNrOfPoints)) };
    return index;
}

fMicroSecs Signal::index2time
(
    Param     const & param,
    SIG_INDEX const   index
) const
{
    if (index < 0)
        return 0.0_MicroSecs;
    float      const fNrOfPoints  { static_cast<float>(index) };
    fMicroSecs const timeTilStart { param.TimeResolution() * fNrOfPoints };
    fMicroSecs const usResult     { timeTilStart + m_timeStart };
    return usResult;
}

mV Signal::GetDataPoint
(
    Param      const & param,
    fMicroSecs const   time
) const
{
    SIG_INDEX index { time2index(param, time) };
    return (index < 0)
           ? mV::NULL_VAL()
           : GetVectorValue<mV>(index, m_data);
}

void Signal::Add(mV const val) 
{
    m_data.push_back(val);
}

void Signal::Notify(bool const bImmediate) // called by compute thread!
{
    Add(m_sigSource.GetSignalValue());
}

fMicroSecs Signal::FindNextMaximum
(
    Param      const & param,
    fMicroSecs const   time
) const
{
    SIG_INDEX index { time2index(param, time) };
    if ((0 > index)||(index >= m_data.size()))
        index = INVALID_SIG_INDEX;
    if ((index > 0) && (m_data[index - 1] > m_data[index])) // falling values, go left
    {   
        while ((--index > 0) && (m_data[index-1] >= m_data[index]));
    }
    else   // climbing values, go right
    {
        while ((index < (m_data.size()-1)) && (m_data[index] <= m_data[index + 1]))
            ++index;
    }
    return index2time(param, index);
}

void Signal::CheckSignal() const 
{
#ifdef _DEBUG
#endif
}

void Signal::Dump() const
{
    m_sigSource.Dump();
    wcout << L"time start: " << m_timeStart << endl;
}
