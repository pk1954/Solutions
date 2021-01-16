// InputNeuron.cpp 
//
// NNetModel

#include "stdafx.h"
#include <iomanip>
#include "DrawContext.h"
#include "tHighlightType.h"
#include "ParameterType.h"
#include "ShapeType.h"
#include "NNetColors.h"
#include "NNetParameters.h"
#include "InputNeuron.h"

using std::chrono::microseconds;
using std::wostringstream;
using std::setprecision;
using std::fixed;

InputNeuron::InputNeuron( MicroMeterPoint const upCenter )
	: Neuron( upCenter, ShapeType::Value::inputNeuron )
{ 
	SetPulseFrequency( STD_PULSE_FREQ );
}

InputNeuron::~InputNeuron( ) { }

bool InputNeuron::operator==( Shape const & rhs ) const
{
	InputNeuron const & inputNeuronRhs { static_cast<InputNeuron const &>(rhs) };
	return 
	( this->Neuron::operator== (inputNeuronRhs) )           &&
	( m_mvFactor       == inputNeuronRhs.m_mvFactor )       &&
	( m_pulseFrequency == inputNeuronRhs.m_pulseFrequency ) &&
	( m_pulseDuration  == inputNeuronRhs.m_pulseDuration );
}

void InputNeuron::Recalc( )
{
	m_mvFactor = mV( m_pParameters->GetParameterValue( ParameterType::Value::peakVoltage ) / m_pulseDuration.GetValue() );
}

fHertz const InputNeuron::SetPulseFrequency( fHertz const freq )
{
	fHertz const fOldValue { m_pulseFrequency };
	m_pulseFrequency = freq;
	m_pulseDuration  = PulseDuration( m_pulseFrequency );
	Recalc( );
	return fOldValue;
}

bool InputNeuron::CompStep( )
{
	m_timeSinceLastPulse += m_pParameters->GetTimeResolution( );
	bool bTrigger { m_timeSinceLastPulse >= m_pulseDuration };
	if ( bTrigger )
		m_timeSinceLastPulse = 0._MicroSecs;   
	return m_bStopOnTrigger && bTrigger;
}

//void InputNeuron::drawInputNeuron
//( 
//	DrawContext   const & context,  
//	D2D1::ColorF  const   colF,
//	float         const   fReductionFactor
//) const
//{
//	MicroMeterPoint const axonVector
//	{
//		HasAxon()
//		? m_connections.GetFirstOutgoing().GetVector( )
//	    : MicroMeterPoint { 0._MicroMeter, 1._MicroMeter } 
//	};
//	MicroMeter      const umHypot    { Hypot( axonVector ) };
//	MicroMeterPoint const umExtVector{ axonVector * (GetExtension() / umHypot) };
//	MicroMeterPoint const umCenter   { GetPosition() };
//	MicroMeterPoint const umStartPnt { umCenter + umExtVector * fReductionFactor };
//	MicroMeterPoint const umEndPnt   { umCenter - umExtVector };
//
//	context.DrawLine( umStartPnt, umEndPnt, GetExtension() * fReductionFactor * 2, colF );
//}
//
//void InputNeuron::DrawExterior( DrawContext const & context, tHighlightType const type ) const
//{
//	drawInputNeuron( context, GetFrameColor( type ), 1.0f );
//}

//void InputNeuron::DrawInterior( DrawContext const & context ) const
//{ 
////	drawInputNeuron( context, GetInteriorColor( ), NEURON_INTERIOR );
//}

void InputNeuron::drawInputShape
( 
	DrawContext  const & context, 
	float        const   M,     // overall width/height                        
	float        const   E,     // vertical offset of end point middle section 
	float        const   W,     // width of left/right section                 
	float        const   S,     // vertical offset of start point (all sections )  
	D2D1::ColorF const   colF
) const
{
	float const V { M * 0.5f }; // vertical offset of endpoint left/right sections  

	MicroMeterPoint const axonVector
	{
		HasAxon()
		? m_connections.GetFirstOutgoing().GetVector( )
		: MicroMeterPoint { 0._MicroMeter, 1._MicroMeter } 
	};
	MicroMeter      const umHypot      { Hypot( axonVector ) };
	MicroMeterPoint const umExtVector  { axonVector * (GetExtension() / umHypot) };
	MicroMeterPoint const umCenter     { GetPosition() };
	MicroMeterPoint const umOrthoVector{ umExtVector.OrthoVector( GetExtension() ) * 0.7f };

	context.DrawLine( umCenter + umExtVector * S                , umCenter - umExtVector * E                , GetExtension() * M, colF );
	context.DrawLine( umCenter + umExtVector * S + umOrthoVector, umCenter - umExtVector * V + umOrthoVector, GetExtension() * W, colF );
	context.DrawLine( umCenter + umExtVector * S - umOrthoVector, umCenter - umExtVector * V - umOrthoVector, GetExtension() * W, colF );
}

void InputNeuron::DrawExterior( DrawContext const & context, tHighlightType const type ) const
{
	drawInputShape( context, 2.0f, 0.2f, 0.6f, 1.0f, GetFrameColor( type ) );
}

void InputNeuron::DrawInterior( DrawContext const & context ) const
{
	drawInputShape( context, 1.6f, 0.0f, 0.2f, 0.8f, GetInteriorColor() );
}
void InputNeuron::DrawNeuronText( DrawContext const & context ) const
{ 
	wostringstream m_wBuffer;

	m_wBuffer.clear( );
	m_wBuffer.str( std::wstring() );
	m_wBuffer << fixed << setprecision(2) 
		      << GetPulseFrequency().GetValue() 
		      << L" " 
		      << ParameterType::GetUnit( ParameterType::Value::pulseRate );

	DisplayText( context, GetRect4Text(), m_wBuffer.str( ) );
}

InputNeuron const * Cast2InputNeuron( Shape const * pShape )
{
	assert( pShape != nullptr );
	assert( pShape->IsInputNeuron() );
	return static_cast<InputNeuron const *>(pShape);
}

InputNeuron * Cast2InputNeuron( Shape * pShape )
{
	assert( pShape != nullptr );
	assert( pShape->IsInputNeuron() );
	return static_cast<InputNeuron *>(pShape);
}
