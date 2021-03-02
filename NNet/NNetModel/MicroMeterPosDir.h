// MicroMeterPosDir.h
//
// NNetModel

#pragma once

#include <vector>
#include <assert.h>   
#include "MoreTypes.h"
#include "PointType.h"

using std::vector;
using std::unique_ptr;
using std::make_unique;

class MicroMeterPosDir
{
public:

    MicroMeterPosDir()
      : m_pos(),
        m_dir()
    {}

    MicroMeterPosDir( MicroMeterPoint const & pos, Degrees const & dir )
        : m_pos(pos),
          m_dir(dir)
    {}

    MicroMeterPoint const GetPos() const { return m_pos; }
    Degrees         const GetDir() const { return m_dir; }

    void SetPos(MicroMeterPoint const & pos) { m_pos = pos; }
    void SetDir(Degrees         const & dir) { m_dir = dir; }

    MicroMeterPosDir& operator+= (MicroMeterPosDir const & rhs) 
    { 
        m_pos += rhs.m_pos;
        m_dir += rhs.m_dir;
        Normalize( m_dir );
        return * this; 
    }

    MicroMeterPosDir& operator-= (MicroMeterPosDir const & rhs) 
    { 
        m_pos -= rhs.m_pos;
        m_dir -= rhs.m_dir;
        Normalize( m_dir );
        return * this; 
    }

    MicroMeterPosDir& operator*= (float const factor) 
    { 
        m_pos *= factor;
        m_dir *= factor;
        Normalize( m_dir );
        return * this; 
    }
    
    friend MicroMeterPosDir const operator+ (MicroMeterPosDir const a, MicroMeterPosDir const b) 
    { 
        MicroMeterPosDir res { a }; 
        res += b; 
        return res; 
    };

    friend MicroMeterPosDir const operator- (MicroMeterPosDir const a, MicroMeterPosDir const b) 
    { 
        MicroMeterPosDir res { a }; 
        res -= b; 
        return res; 
    };

    friend MicroMeterPosDir const operator* (MicroMeterPosDir const a, float const f) 
    { 
        MicroMeterPosDir res { a }; 
        res *= f; 
        return res; 
    };

private:
    MicroMeterPoint m_pos;
    Degrees         m_dir;
};