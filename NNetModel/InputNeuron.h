// InputNeuron.h
//
// NNetModel

#pragma once

#include <chrono>
#include "MoreTypes.h"
#include "BaseKnot.h"

using std::chrono::microseconds;

class Pipeline;

class InputNeuron : public BaseKnot
{
public:

	InputNeuron( MicroMeterPoint const );

	virtual void Prepare( NNetModel & );
	virtual void Step( );
	virtual mV   GetNextOutput( ) const;

	void    Trigger( );
	PERCENT GetFillLevel( ) const;
	virtual void DrawExterior( NNetModel const &, GraphicsInterface &, PixelCoordsFp const & ) const;
	virtual void DrawInterior( NNetModel const &, GraphicsInterface &, PixelCoordsFp const & ) const;

	Hertz GetPulseFrequency( ) const
	{
		return m_pulseFrequency;
	}

	void SetPulseFrequency( Hertz const );

private:

	microseconds m_timeSinceLastPulse;
	Hertz        m_pulseFrequency;

	mV waveFunction( microseconds ) const;
};	

InputNeuron const * Cast2InputNeuron( Shape const * );
InputNeuron       * Cast2InputNeuron( Shape       * );
