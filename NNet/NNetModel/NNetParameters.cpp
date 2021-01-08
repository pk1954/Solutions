// NNetParameters.cpp
//
// NNetModel

#include "stdafx.h"
#include "NNetParameters.h"

bool Param::operator==( Param const & rhs ) const
{
	return
		(m_threshold     == rhs.m_threshold    ) && 
		(m_peakVoltage   == rhs.m_peakVoltage  ) &&
		(m_pulseWidth    == rhs.m_pulseWidth   ) &&
		(m_refractPeriod == rhs.m_refractPeriod) &&
		(m_pulseSpeed    == rhs.m_pulseSpeed   ) &&
		(m_usResolution  == rhs.m_usResolution );
}

float const Param::GetParameterValue( tParameter const param ) const
{
	switch ( param )
	{
		case tParameter::pulseSpeed:	 return m_pulseSpeed.GetValue();
		case tParameter::pulseWidth:	 return m_pulseWidth.GetValue();
		case tParameter::threshold:  	 return m_threshold.GetValue();
		case tParameter::peakVoltage:	 return m_peakVoltage.GetValue();
		case tParameter::refractPeriod:  return m_refractPeriod.GetValue();
		case tParameter::timeResolution: return m_usResolution.GetValue();
		case tParameter::signalLoss:     return 0.0f;  // no longer used, only for backward compatibility
		default: assert( false );
	}
	return 0.f;
}

void Param::SetParameterValue
( 
	tParameter const param, 
	float      const fNewValue 
)
{
	switch ( param )
	{
		case tParameter::pulseSpeed:	 m_pulseSpeed    = static_cast< meterPerSec >( fNewValue ); break;
		case tParameter::pulseWidth:	 m_pulseWidth    = static_cast< fMicroSecs  >( fNewValue ); break;
		case tParameter::threshold:		 m_threshold     = static_cast< mV >         ( fNewValue ); break;
		case tParameter::peakVoltage:	 m_peakVoltage   = static_cast< mV >         ( fNewValue ); break;
		case tParameter::refractPeriod:	 m_refractPeriod = static_cast< fMicroSecs  >( fNewValue ); break;
		case tParameter::timeResolution: m_usResolution  = static_cast< fMicroSecs  >( fNewValue ); break;
		case tParameter::signalLoss: break; // legacy
		default: assert( false );
	}

	NotifyAll( false );
}
