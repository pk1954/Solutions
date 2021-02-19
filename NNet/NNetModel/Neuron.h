// Neuron.h
//
// NNetModel

#pragma once

#include <chrono>
#include "MoreTypes.h"
#include "NNetParameters.h"
#include "SoundInterface.h"
#include "tHighlightType.h"
#include "Pipe.h"
#include "BaseKnot.h"

class DrawContext;

using std::unique_ptr;
using std::make_unique;

class Neuron : public BaseKnot
{
public:
	Neuron( MicroMeterPoint const, ShapeType const = ShapeType::Value::neuron );

	Neuron( Neuron const & );   // copy constructor

	Neuron & operator=( Neuron const & ); // copy assignment operator

	virtual ~Neuron();

	virtual bool operator==( Shape const & ) const override;

	static unsigned long GetCounter  ( ) { return m_counter; }
	static   void        ResetCounter( ) { m_counter = 0L; }
	virtual  void        IncCounter  ( ) { ++ m_counter; }
	virtual  void        DecCounter  ( ) { -- m_counter; }

	static bool TypeFits( ShapeType const type ) { return type.IsAnyNeuronType( ); }

	bool       const HasAxon         ( ) const { return m_connections.HasOutgoing(); }
	bool       const HasTriggerSound ( ) const { return m_triggerSound.m_bOn; }
	SoundDescr const GetTriggerSound ( ) const { return m_triggerSound; }

	SoundDescr const SetTriggerSound( SoundDescr const & );

	fMicroSecs   PulseWidth   () const;
	fMicroSecs   RefractPeriod() const;
	mV           Threshold    () const;
	mV           PeakVoltage  () const;

	void         StopOnTrigger(tBoolOp const op) { ApplyOp( m_bStopOnTrigger, op ); }

	virtual void SetDirection(MicroMeterPoint const &);
		
	virtual void DrawRectExterior( DrawContext const &, tHighlightType const ) const;
	virtual void DrawRectInterior( DrawContext const &, tHighlightType const ) const;

	virtual void DrawExterior  ( DrawContext const &, tHighlightType const) const;
	virtual void DrawInterior  ( DrawContext const &, tHighlightType const) const;
	virtual void DrawNeuronText( DrawContext const & ) const;
	virtual void Recalc( );
	virtual void Clear( );
	virtual bool CompStep( );
	virtual mV   GetNextOutput( ) const;

	static void SetSound( Sound * const pSound ) { m_pSound = pSound; }

protected:
	MicroMeterPoint m_umVector { MicroMeterPoint::NULL_VAL() };  // direction of neuron if plug or socket

	fMicroSecs m_timeSinceLastPulse { 0._MicroSecs };
	bool       m_bStopOnTrigger     { false };

	mV waveFunction( fMicroSecs const ) const;

	void const DisplayText( DrawContext const &, MicroMeterRect const &, wstring const ) const;

private:
	mutable bool m_bTriggered { false };

	float m_factorW; // Parameter of wave function
	float m_factorU; // Parameter of wave function

	SoundDescr m_triggerSound {};

	PTP_WORK  m_pTpWork { nullptr };  // Thread poolworker thread
	MicroMeterPoint getAxonHillockPos( ) const;

	void drawPlug
	( 
		DrawContext  const &, 
		float        const, 
		float        const, 
		D2D1::ColorF const
	) const;

	inline static unsigned long m_counter { 0L };
	inline static Sound       * m_pSound  { nullptr };

	void init( const Neuron & );

	friend static void CALLBACK BeepFunc( PTP_CALLBACK_INSTANCE, PVOID,	PTP_WORK );
};

Neuron const * Cast2Neuron( Shape const * );
Neuron       * Cast2Neuron( Shape       * );
