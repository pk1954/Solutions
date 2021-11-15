// SignalGenerator.cpp 
//
// NNetModel

#include "stdafx.h"
#include "NNetParameters.h"
#include "SignalGenerator.h"

SignalGenerator::SignalGenerator()
{
	LoadParameterValues();
}

void SignalGenerator::TriggerStimulus()
{
	m_usSinceLastStimulus = 0._MicroSecs;
	m_bTriggerActive = true;
	NotifyAll(false);
}

fHertz const SignalGenerator::GetFrequency(fMicroSecs const uSecs) const
{
	if (InStimulusRange(uSecs))
	{
		float  const fFactor  { uSecs / m_usMax };
		fHertz const freqStim { m_freqMaxStim * exp(1 - fFactor) * fFactor };
		return m_freqBase + freqStim;
	}
	else
		return m_freqBase;
}

fHertz const SignalGenerator::GetActFrequency() const 
{ 
	return GetFrequency(m_usSinceLastStimulus);
}

void SignalGenerator::Tick()
{
	m_usSinceLastStimulus += m_pParameters->TimeResolution();
	if (m_bTriggerActive && ! InStimulusRange(m_usSinceLastStimulus))
		StopTrigger();
	NotifyAll(false);
}

void SignalGenerator::LoadParameterValues()
{
	m_freqBase    = m_pParameters->BaseFrequency();
	m_freqMaxStim = m_pParameters->StimulusMaxFreq() - m_freqBase;
	m_usMax       = m_pParameters->StimulusMaxTime();
}

//void SignalGenerator::SetParameterValues()
//{
//	m_pParameters->SetParameterValue(ParamType::Value::baseFrequency,   m_freqBase.GetValue());
//	m_pParameters->SetParameterValue(ParamType::Value::stimulusMaxFreq, (m_freqMaxStim + m_freqBase).GetValue());
//	m_pParameters->SetParameterValue(ParamType::Value::stimulusMaxTime, m_usMax.GetValue());
//}

void SignalGenerator::SetParam(ParamType::Value const par, float const f)
{
	switch ( par )
	{
	case ParamType::Value::stimulusMaxFreq:
		m_freqMaxStim = f;
		break;

	case ParamType::Value::stimulusMaxTime:
		m_usMax = f;
		break;

	case ParamType::Value::baseFrequency:
		m_freqBase = f;
		break;

	default:
		assert(false);
	}
}

