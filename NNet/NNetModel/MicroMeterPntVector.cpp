// MicroMeterPntVector.cpp
//
// NNetModel

#include "stdafx.h"
#include "MicroMeterPntVector.h"

MicroMeterPntVector::MicroMeterPntVector(NobPtrList<IoNeuron> const& nobList)
{
    nobList.Apply2All( [&](IoNeuron const & c) { Add( c.GetPosDir() ); } );
}

void MicroMeterPntVector::Apply2All(function<void(MicroMeterPosDir &)> const & func)
{
    for (auto & elem: m_list)
        func(elem);
}

unsigned int const MicroMeterPntVector::Size() const 
{ 
    return Cast2UnsignedInt(m_list.size()); 
}

MicroMeterPosDir const MicroMeterPntVector::GetPosDir( unsigned int const ui) const
{
    assert( ui < Size() );
    return m_list[ui];
}

void MicroMeterPntVector::SetPosDir(unsigned int const ui, MicroMeterPosDir const & posDir)
{
    assert( ui < Size() );
    m_list[ui] = posDir;
}

void MicroMeterPntVector::SetDir(Radian const radian)
{
    Apply2All( [&](MicroMeterPosDir & umPosDir) { umPosDir.SetDir(radian); } );
}

void MicroMeterPntVector::SetDir(MicroMeterPntVector const rhs)
{
    assert(rhs.Size() == Size());
    for ( int i = 0; i < m_list.size(); ++i )
        m_list[i].SetDir( rhs.GetPosDir(i).GetDir() );
}

void MicroMeterPntVector::SetPos(MicroMeterPntVector const rhs)
{
    assert(rhs.Size() == Size());
    for ( int i = 0; i < m_list.size(); ++i )
        m_list[i].SetPos( rhs.GetPosDir(i).GetPos() );
}

void MicroMeterPntVector::Clear()
{
    m_list.clear();
}

void MicroMeterPntVector::Resize( size_t const size )
{
    m_list.resize( size );
}

void MicroMeterPntVector::Add( MicroMeterPnt const & pos, Radian const dir )
{
    m_list.push_back( MicroMeterPosDir(pos, dir) );
}

void MicroMeterPntVector::Add( MicroMeterPosDir const & posDir )
{
    m_list.push_back( posDir );
}

bool MicroMeterPntVector::operator==( MicroMeterPntVector const& rhs ) const
{
    assert( m_list.size() == rhs.m_list.size() );
    for ( int i = 0; i < m_list.size(); ++i )
        if (m_list[i] != rhs.m_list[i])
            return false;
    return true; 
}

MicroMeterPntVector& MicroMeterPntVector::operator+= (MicroMeterPntVector const & rhs) 
{ 
    assert( m_list.size() == rhs.m_list.size() );
    for ( int i = 0; i < m_list.size(); ++i )
        m_list[i] += rhs.m_list[i];
    return * this; 
}

MicroMeterPntVector& MicroMeterPntVector::operator-= (MicroMeterPntVector const & rhs) 
{ 
    assert( m_list.size() == rhs.m_list.size() );
    for ( int i = 0; i < m_list.size(); ++i )
        m_list[i] -= rhs.m_list[i];
    return * this; 
}

MicroMeterPntVector& MicroMeterPntVector::operator*= (float const factor) 
{ 
    for ( int i = 0; i < m_list.size(); ++i )
        m_list[i] *= factor;
    return * this; 
}

MicroMeterPntVector const operator+ (MicroMeterPntVector const a, MicroMeterPntVector const b) 
{ 
    MicroMeterPntVector res { a }; 
    res += b; 
    return res; 
};

MicroMeterPntVector const operator- (MicroMeterPntVector const a, MicroMeterPntVector const b) 
{ 
    MicroMeterPntVector res { a }; 
    res -= b; 
    return res; 
};

MicroMeterPntVector const operator* (MicroMeterPntVector const a, float const f) 
{ 
    MicroMeterPntVector res { a }; 
    res *= f; 
    return res; 
};

wostream & operator<< (wostream & out, MicroMeterPntVector const & v)
{
    out << MicroMeterPntVector::OPEN_BRACKET << v.m_list.size() << L":";
    for ( auto & it: v.m_list )
    {
        out << it;
        if ( &it == &v.m_list.back() )
            break;
        out << MicroMeterPntVector::SEPARATOR;
    }
    out << MicroMeterPntVector::CLOSE_BRACKET;
    return out; 
}

Radian const MicroMeterPntVector::FindMaxRadian() const 
{
    if ( m_list.empty() )
        return Radian::NULL_VAL();
    MicroMeterPosDir const maxElement = * std::max_element
    (
        m_list.begin(), m_list.end(), 
        [](MicroMeterPosDir const & a, MicroMeterPosDir const & b)
        { 
            return a.GetDir() < b.GetDir(); 
        }
    );
    return maxElement.GetDir();
}

MicroMeter const MicroMeterPntVector::FindMaxPos() const
{
    if ( m_list.empty() )
        return MicroMeter::NULL_VAL();
    MicroMeterPosDir const maxElement = * std::max_element
    (
        m_list.begin(), m_list.end(), 
        [](MicroMeterPosDir const & a, MicroMeterPosDir const & b)
        { 
            return Hypot(a.GetPos()) < Hypot(b.GetPos()); 
        }
    );
    return Hypot(maxElement.GetPos());
}

MicroMeterLine const MicroMeterPntVector::GetLine()
{
    return MicroMeterLine(m_list.front().GetPos(), m_list.back().GetPos());
}

void MicroMeterPntVector::Align( MicroMeterPnt const& umPntStart, MicroMeterPnt const& umPntOffset )
{	
    MicroMeterPnt umPnt { umPntStart };
    Apply2All
    (	
        [&](MicroMeterPosDir & posDir)	
        { 
            posDir.SetPos(umPnt);
            umPnt += umPntOffset;
        }	
    );
}

void MicroMeterPntVector::Align( MicroMeterLine const & umLine, MicroMeter umDist )
{
    MicroMeterPnt const umVector           { umLine.GetVector() };
    MicroMeter    const umLineLengthTarget { umDist * gapCount() };
    MicroMeterPnt const umPntSingleVector  { umVector.ScaledTo(umDist) };
    MicroMeterPnt const umPntLineTarget    { umVector.ScaledTo(umLineLengthTarget) };
    MicroMeterPnt const umPntTargetStart   { umLine.GetCenter() - umPntLineTarget * 0.5f };
    Align(umPntTargetStart, umPntSingleVector);
}

void MicroMeterPntVector::Align( MicroMeterLine const umLine )
{
    Align(umLine, umLine.Length() / gapCount());
}

void MicroMeterPntVector::Pack( MicroMeter umDist )
{
    Align(GetLine(), umDist);
}

unsigned int const CalcNrOfSteps
(
    MicroMeterPntVector const & umPntVectorStart,
    MicroMeterPntVector const & umPntVectorTarget
)
{
    MicroMeterPntVector const umPntVectorDiff { umPntVectorTarget - umPntVectorStart };
    float               const fStepsFromRot   { CalcNrOfSteps(umPntVectorDiff.FindMaxRadian()) };
    float               const fStepsFromMove  { CalcNrOfSteps(umPntVectorDiff.FindMaxPos()) };
    float               const fSteps          { max(fStepsFromRot, fStepsFromMove) };
    unsigned int        const uiSteps         { Cast2UnsignedInt(fSteps) + 1 };
    return uiSteps;
}