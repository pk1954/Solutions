// Shape.h
//
// NNetModel

#pragma once

#include "MoreTypes.h"
#include "NNetParameters.h"

using ShapeId = NamedType< long, struct ShapeIdParam >;

class GraphicsInterface;
class PixelCoordsFp;
class NNetModel;

ShapeId const NO_SHAPE( -1 );

enum class tShapeType
{
	undefined,
	inputNeuron,
	outputNeuron,
	neuron,
	pipeline,
	knot
};

static bool IsDefined( ShapeId const id )
{
	return id != NO_SHAPE;
}

static bool IsBaseKnotType( tShapeType const type )
{
	return 
		(type == tShapeType::knot)        || 
		(type == tShapeType::neuron)      || 
		(type == tShapeType::inputNeuron) ||
		(type == tShapeType::outputNeuron);
}

static bool IsNeuronType( tShapeType const type )
{
	return  
		(type == tShapeType::neuron)      || 
		(type == tShapeType::inputNeuron) ||
		(type == tShapeType::outputNeuron);
}

class Shape
{
public:
	Shape( tShapeType const type )
	  :	m_mVinputBuffer( 0._mV ),
		m_bHighlighted( false ),
		m_bSuperHighlighted( false ),
		m_identifier( NO_SHAPE ),
		m_type( type )
	{
	}

	virtual void DrawExterior  ( NNetModel const &, GraphicsInterface &, PixelCoordsFp const & ) const = 0;
	virtual void DrawInterior  ( NNetModel const &, GraphicsInterface &, PixelCoordsFp const & ) const = 0;
	virtual bool IsPointInShape( NNetModel const &, MicroMeterPoint const & )                    const = 0;
	virtual void MoveTo        ( NNetModel       &, MicroMeterPoint const & )                          = 0;
	virtual void Prepare       ( NNetModel       & )                                                   = 0;
	virtual void Step( )                                                                               = 0;
	virtual mV   GetNextOutput( )                                                                const = 0;

	void SetHighlightState( bool const bState )
	{
		m_bHighlighted = bState;
	}

	void SetSuperHighlightState( bool const bState )
	{
		m_bSuperHighlighted = bState;
	}

	bool IsHighlighted( ) const
	{
		return m_bHighlighted;
	}

	bool IsSuperHighlighted( ) const
	{
		return m_bSuperHighlighted;
	}

	ShapeId GetId( ) const
	{
		return m_identifier;
	}

	void SetId( ShapeId const id )
	{
		m_identifier = id;
	}

	tShapeType GetShapeType( ) const
	{
		return m_type;
	}

	float GetFillLevel( ) const  // 1.0 means 100% filled
	{
		float res = m_mVinputBuffer / PEAK_VOLTAGE;
//		assert( res <= 1.0f );
		return res;
	}

protected:
	mV m_mVinputBuffer;

private:
	ShapeId    m_identifier;
	bool       m_bHighlighted;
	bool       m_bSuperHighlighted;
	tShapeType m_type;
};
