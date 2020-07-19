// Neuron.h
//
// NNetModel

#pragma once

#include <chrono>
#include "MoreTypes.h"
#include "NNetParameters.h"
#include "win32_sound.h"
#include "tHighlightType.h"
#include "Pipe.h"
#include "BaseKnot.h"

class DrawContext;

class Neuron : public BaseKnot
{
public:
	Neuron( MicroMeterPoint const, ShapeType const = ShapeType::Value::neuron );
	virtual ~Neuron();

	virtual bool IsEqual( Neuron const & other ) const
	{
		if ( ! BaseKnot::IsEqual( other ) )
			return false;
		if ( m_bTriggered != other.m_bTriggered )
			return false;
		if ( m_factorW != other.m_factorW )
			return false;
		if ( m_factorU != other.m_factorU )
			return false;
		if ( m_triggerSound.m_bOn != other.m_triggerSound.m_bOn )
			return false;
		if ( m_triggerSound.m_frequency != other.m_triggerSound.m_frequency )
			return false;
		if ( m_triggerSound.m_duration != other.m_triggerSound.m_duration )
			return false;
		return true;
	}

	static unsigned long GetCounter( ) { return m_counter; }

	static bool TypeFits( ShapeType const type ) { return type.IsAnyNeuronType( ); }

	bool       const HasAxon                 ( ) const { return m_connections.HasOutgoing();	}
	bool       const HasTriggerSound         ( ) const { return m_triggerSound.m_bOn; }
	//Hertz      const GetTriggerSoundFrequency( ) const { return m_triggerSound.m_frequency; }
	//MilliSecs  const GetTriggerSoundDuration ( ) const { return m_triggerSound.m_duration; }
	SoundDescr const GetTriggerSound         ( ) const { return m_triggerSound; }

	SoundDescr const SetTriggerSound( SoundDescr const & );

	fMicroSecs PulseWidth   ( ) const;
	fMicroSecs RefractPeriod( ) const;
	mV         Threshold    ( ) const;
	mV         PeakVoltage  ( ) const;

	void       StopOnTrigger( tBoolOp const op ) { ApplyOp( m_bStopOnTrigger, op ); }

	virtual void DrawExterior( DrawContext const &, tHighlightType const  = tHighlightType::normal ) const;
	virtual void DrawInterior( DrawContext const & ) const;
	virtual void DrawNeuronText( DrawContext const & ) const;
	virtual void Recalc( );
	virtual void Clear( );
	virtual bool CompStep( );
	virtual mV   GetNextOutput( ) const;

protected:
	fMicroSecs m_timeSinceLastPulse { 0._MicroSecs };
	bool       m_bStopOnTrigger     { false };

	mV waveFunction( fMicroSecs const ) const;

	void const DisplayText( DrawContext const &, MicroMeterRect const &, wstring const ) const;

private:
	mutable bool m_bTriggered { false };

	float     m_factorW; // Parameter of wave function
	float     m_factorU; // Parameter of wave function

	SoundDescr m_triggerSound;
	//bool      m_bTriggerSoundOn       { false };
	//Hertz     m_triggerSoundFrequency { 0_Hertz };   
	//MilliSecs m_triggerSoundDuration  { 0_MilliSecs };

	PTP_WORK  m_pTpWork { nullptr };

	MicroMeterPoint getAxonHillockPos( ) const;

	inline static unsigned long m_counter { 0L };
};

Neuron const * Cast2Neuron( Shape const * );
Neuron       * Cast2Neuron( Shape       * );
