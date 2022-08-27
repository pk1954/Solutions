// Sensor.ixx
//
// NNetModel

module;

#include <vector>
#include <iostream>
#include "Pipe.h"
#include "UPNobList.h"
#include "SignalSource.h"

export module Sensor;

import BasicTypes;
import MoreTypes;
import AngleTypes;
import NNetParameters;
import DrawContext;

using std::vector;
using std::wostream;

export class Sensor : public SignalSource
{
public:
    Sensor(MicroMeterCircle const &, UPNobList const &);

    void Dump()                                const final;
    mV   GetSignalValue()                      const final;
    void WriteInfo(wostream &)                 const final;
    void Draw(DrawContext const &, bool const) const final;
    bool Includes(MicroMeterPnt const p)       const final { return m_circle.Includes(p); }

    void  DrawDataPoints(DrawContext  const &) const;
    float GetDistFactor(MicroMeterPnt const &) const;

    MicroMeterPnt    const & GetCenter()       const { return m_circle.GetPos(); }
    MicroMeter               GetRadius()       const { return m_circle.GetRadius(); }
    MicroMeterCircle const & GetCircle()       const { return m_circle; }
    size_t                   GetNrOfElements() const { return m_dataPoints.size(); }

    void SetSensorPos (UPNobList const &, MicroMeterPnt const &);
    void MoveSensor   (UPNobList const &, MicroMeterPnt const &);
    void SetSensorSize(UPNobList const &, MicroMeter    const);
    void SizeSensor   (UPNobList const &, float         const);
    void RotateSensor (MicroMeterPnt const &, Radian const);

private:

    struct SigDataPoint
    {
        Pipe          const * m_pPipe;
        Pipe::SegNr   const   m_segNr;
        MicroMeterPnt const   m_umPos;
        float         const   m_fFactor;

        mV GetSignalValue() const
        {
            return m_pPipe->GetVoltage(m_segNr) * m_fFactor;
        }

        MicroMeterCircle dataPointCircle() const
        {
            return MicroMeterCircle(m_umPos, PIPE_WIDTH * 0.5f);
        }
    };

    MicroMeterCircle     m_circle{ MicroMeterCircle::NULL_VAL() };
    vector<SigDataPoint> m_dataPoints{ };

    void Recalc(UPNobList const &) final;

    void add2list(Pipe const &);

    SigDataPoint const * findDataPoint(MicroMeterPnt const &) const;
};