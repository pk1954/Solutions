// MicroMeterPosDir.h
//
// NNetModel

#pragma once

#include <vector>
#include <assert.h>   
#include "MoreTypes.h"

import PointType;

using std::vector;
using std::unique_ptr;
using std::make_unique;

class Nob;

class MicroMeterPosDir
{
public:

    MicroMeterPosDir();
    MicroMeterPosDir(MicroMeterPnt const &, Radian const &);

    MicroMeterPosDir(Nob const &);
    
    MicroMeterPnt GetPos() const { return m_pos; }
    Radian        GetDir() const { return m_dir; }

    void SetPos(MicroMeterPnt const & pos) { m_pos = pos; }
    void SetDir(Radian        const & dir) { m_dir = dir; }

    bool operator==(MicroMeterPosDir const&) const = default;

    MicroMeterPosDir& operator+= (MicroMeterPnt const &);
    MicroMeterPosDir& operator+= (MicroMeterPosDir const &); 
    MicroMeterPosDir& operator-= (MicroMeterPosDir const &); 
    MicroMeterPosDir& operator*= (float const); 

    friend MicroMeterPosDir operator+ (MicroMeterPosDir const, MicroMeterPosDir const); 
    friend MicroMeterPosDir operator- (MicroMeterPosDir const, MicroMeterPosDir const); 
    friend MicroMeterPosDir operator* (MicroMeterPosDir const, float const); 

    friend wostream & operator<< (wostream &, MicroMeterPosDir const &);

    static MicroMeterPosDir const & NULL_VAL() 
    { 
        static MicroMeterPosDir res 
        { 
            MicroMeterPosDir(MicroMeterPnt::NULL_VAL(), 
            Radian::NULL_VAL()) 
        }; 
        return res;
    };

    friend MicroMeter Hypot(MicroMeterPosDir const & p) { return Hypot(p.GetPos()); }

private:
    MicroMeterPnt m_pos;
    Radian        m_dir;
};

unsigned int CalcNrOfSteps(MicroMeterPosDir const &, MicroMeterPosDir const &);
