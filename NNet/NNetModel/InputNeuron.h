// InputNeuron.h
//
// NNetModel

#pragma once

#include "MoreTypes.h"
#include "ConnectionNeuron.h"

class InputNeuron : public ConnNeuron
{
public:

	InputNeuron( MicroMeterPoint const );

	virtual ~InputNeuron();

	virtual void CheckShape() const;

	virtual bool operator==( Shape const & ) const override;

	static bool      const TypeFits( ShapeType const type ) { return type.IsInputNeuronType(); }
	static ShapeType const GetShapeType() { return ShapeType::Value::inputNeuron; }

	virtual void DrawExterior( DrawContext const &, tHighlight const ) const;
	virtual void DrawInterior( DrawContext const &, tHighlight const ) const;

	virtual void Prepare()
	{
		m_mVinputBuffer = m_mvFactor * m_timeSinceLastPulse.GetValue();
	}

	virtual bool const CompStep();
	virtual void       DrawNeuronText( DrawContext const & ) const;
	virtual void       Recalc();   // Recalculate precalculated values

	fHertz const GetPulseFrequency() const { return m_pulseFrequency; }
	fHertz const SetPulseFrequency( fHertz const );

	virtual bool const Includes( MicroMeterPoint const & ) const;

private:

	MicroMeterPoint const getOffset() const;
	MicroMeterPoint const getCenter() const;

	void drawSocket
	( 
		DrawContext  const &, 
		float        const, 
		float        const, 
		D2D1::ColorF const
	) const;

	mV         m_mvFactor;       // precomputed value for optimization
	fHertz     m_pulseFrequency; // pulse frequency and pulse duration depend on each other
	fMicroSecs m_pulseDuration;  // in principle one variable would be enough, but to avoid 
                                 // floating point rounding effects, both are stored
};	
