// InputNeuron.cpp 
//
// NNetModel

#include "stdafx.h"
#include <iomanip>
#include <sstream> 
#include "DrawContext.h"
#include "tHighlightType.h"
#include "ParameterType.h"
#include "NobType.h"
#include "NNetColors.h"
#include "NNetParameters.h"
#include "Knot.h"
#include "InputNeuron.h"

using std::chrono::microseconds;
using std::wostringstream;
using std::setprecision;
using std::fixed;

InputNeuron::InputNeuron(MicroMeterPnt const & upCenter)
	: IoNeuron(upCenter, NobType::Value::inputNeuron)
{ 
	initialize();
}

InputNeuron::InputNeuron(BaseKnot const & baseKnot)
	: IoNeuron(baseKnot, NobType::Value::inputNeuron)
{ 
	initialize();
	SetOutgoing(baseKnot);
}

InputNeuron::~InputNeuron() 
{
}

void InputNeuron::initialize()
{
}

void InputNeuron::Check() const
{
	Neuron::Check();
	assert(!HasIncoming());
}

//bool InputNeuron::operator==(Nob const & rhs) const
//{
//	InputNeuron const & inputNeuronRhs { static_cast<InputNeuron const &>(rhs) };
//	return (this->Neuron::operator== (inputNeuronRhs)) &&
//		   (m_signalGenerator == inputNeuronRhs.m_signalGenerator);
//}

fHertz const InputNeuron::GetBaseFrequency() const 
{ 
//	return m_pSignalGenerator ? m_pSignalGenerator->GetBaseFrequency() : fHertz::NULL_VAL(); 
	return m_signalGenerator.GetBaseFrequency(); 
}

fHertz const InputNeuron::SetBaseFrequency(fHertz const freq)
{
	return m_signalGenerator.SetBaseFrequency(freq);
}

void InputNeuron::Prepare()
{
	m_mVinputBuffer += m_signalGenerator.GetPotIncrease();
}

bool const InputNeuron::CompStep()
{
	IncreaseTimeSinceLastPulse();
	bool bTrigger { m_mVinputBuffer >= Threshold() };
	if (bTrigger)
		ResetTimeSinceLastPulse();   
	return m_bStopOnTrigger && bTrigger;
}

void InputNeuron::DrawExterior(DrawContext const & context, tHighlight const type) const
{
	drawSocket(context, 2.0f, 0.1f, GetExteriorColor(type));
}

void InputNeuron::DrawInterior(DrawContext const & context, tHighlight const type) const
{
	drawSocket(context, 1.6f, 0.0f, GetInteriorColor(type));
}

MicroMeterPnt const InputNeuron::getOffset() const
{
	return GetScaledDirVector() * 0.7f;
}

MicroMeterPnt const InputNeuron::getCenter() const
{
	return GetPos() - getOffset();
}

bool const InputNeuron::Includes(MicroMeterPnt const & point) const
{
	return Distance(point, getCenter()) <= GetExtension();
}

void InputNeuron::drawSocket
(
	DrawContext  const & context, 
	float        const   M,       // overall width/height                        
	float        const   VEM,     // vertical offset of end point middle section 
	D2D1::ColorF const   colF
) const
{
	MicroMeterPnt  const umDirVector   { GetDirVector() * M };
	MicroMeter     const umSize        { GetExtension() * M };
	MicroMeterPnt  const umOrthoVector { getOffset().OrthoVector() };
	MicroMeterPnt  const umDirVectorVS { umDirVector * 0.5f };
	MicroMeter     const umWidthLR     { umSize - GetExtension() * 1.4f };  // width of left/right section                 
	MicroMeterPnt  const umCenter      { getCenter() };
	MicroMeterPnt  const umStart       { umCenter + umDirVectorVS };
	MicroMeterPnt  const umEndLR       { umCenter - umDirVectorVS };
	MicroMeterPnt  const umEndCenter   { umCenter - umDirVector * VEM };
	MicroMeterLine const umLine        { umStart, umEndLR };

	context.DrawLine(umStart, umEndCenter,   umSize,    colF);
	context.DrawLine(umLine + umOrthoVector, umWidthLR, colF);
	context.DrawLine(umLine - umOrthoVector, umWidthLR, colF);
}

void InputNeuron::DrawNeuronText(DrawContext const & context) const
{ 
	wostringstream m_wBuffer;

	m_wBuffer.clear();
	m_wBuffer.str(std::wstring());
	m_wBuffer << fixed << setprecision(2) 
		      << GetBaseFrequency().GetValue() 
		      << L" " 
		      << ParamType::GetUnit(ParamType::Value::pulseRate);

	DisplayText(context, GetRect4Text(), m_wBuffer.str());
}

