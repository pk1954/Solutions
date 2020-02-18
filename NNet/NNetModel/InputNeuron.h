// InputNeuron.h
//
// NNetModel

#pragma once

#include <chrono>
#include <sstream> 
#include "MoreTypes.h"
#include "NNetModel.h"
#include "Neuron.h"

class InputNeuron : public Neuron
{
public:

	InputNeuron( NNetModel *, MicroMeterPoint const );
	virtual ~InputNeuron( );

	static bool TypeFits( tShapeType const type )
	{
		return type == tShapeType::inputNeuron;
	}

	virtual void Prepare( )
	{
		m_mVinputBuffer = m_mvFactor * m_timeSinceLastPulse.GetValue();
	}

	virtual void Step( )
	{
		if ( m_timeSinceLastPulse >= m_pulseDuration )  
		{
			m_timeSinceLastPulse = 0._MicroSecs;   
		}
		else
		{
			m_timeSinceLastPulse += m_pNNetModel->GetTimeResolution( );
		}
	}

	virtual void DrawExterior  ( PixelCoordsFp &, tHighlightType const ) const;
	virtual void DrawInterior  ( PixelCoordsFp & ) const;
	virtual void DrawNeuronText( PixelCoordsFp & ) const;
	virtual void Recalc( );   // Recalculate precalculated values

	fHertz GetPulseFrequency( ) const {	return m_pulseFrequency; }

	void SetPulseFrequency( fHertz const );

private:
	void drawInputNeuron( PixelCoordsFp const &, COLORREF const, float const ) const;

	mV        m_mvFactor;       // precomputed value for optimization
	fHertz    m_pulseFrequency; // pulse frequency and pulse duration depend on each other
	MicroSecs m_pulseDuration;  // in principle one variable would be enough, but to avoid 
	                            // floating point rounding effects, both are stored
};	

InputNeuron const * Cast2InputNeuron( Shape const * );
InputNeuron       * Cast2InputNeuron( Shape       * );