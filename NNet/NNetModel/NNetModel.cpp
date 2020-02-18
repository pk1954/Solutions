// NNetModel.cpp 
//
// NNetModel

#include "stdafx.h"
#include <vector>
#include <unordered_map>
#include "MoreTypes.h"
#include "NNetParameters.h"
#include "tHighlightType.h"
#include "Knot.h"
#include "Neuron.h"
#include "InputNeuron.h"
#include "NNetModel.h"

#ifndef NDEBUG
	#define CHECK_CONSISTENCY CheckConsistency()
#else
	#define CHECK_CONSISTENCY
#endif

using namespace std::chrono;
using std::unordered_map;

CRITICAL_SECTION NNetModel::m_criticalSection;
bool             NNetModel::m_bCritSectReady = false;

NNetModel::NNetModel( )
  : m_Shapes( ),
	m_timeStamp      ( 0._MicroSecs ),
	m_threshold      ( 20._mV ),
	m_peakVoltage    ( 10._mV ),
	m_pulseWidth     ( 2000._MicroSecs ),
	m_refractPeriod  ( 500._MicroSecs ),
	m_pulseSpeed     ( 0.1_meterPerSec ),
	m_usResolution   ( 100._MicroSecs ),
	m_bUnsavedChanges( false )
{					
	if ( ! m_bCritSectReady )
	{
		(void)InitializeCriticalSectionAndSpinCount( & m_criticalSection, 0x00000400 );
		m_bCritSectReady = true;
	}

	CreateInitialShapes( );
}

NNetModel::~NNetModel( )
{
	m_parameterObservable.UnregisterAllObservers();

	if ( m_bCritSectReady )
	{
		DeleteCriticalSection( & m_criticalSection );
		m_bCritSectReady = false;
	}
}

void NNetModel::CreateInitialShapes( )
{
	InputNeuron * const pInputNeuron { NewShape<InputNeuron >( MicroMeterPoint( 400.0_MicroMeter, 200.0_MicroMeter ) ) };
	Neuron      * const pNeuron      { NewShape<Neuron>      ( MicroMeterPoint( 400.0_MicroMeter, 800.0_MicroMeter ) ) };
	NewPipeline( pInputNeuron, pNeuron );
}

void NNetModel::RecalcAllShapes( ) 
{ 
	Apply2All<Shape>( [&]( Shape & shape ) { shape.Recalc( ); return false; } );
} 

long const NNetModel::GetNrOfShapes( ) const
{
	long lCounter = 0;
	Apply2All<Shape>( [&]( Shape & shape ) { ++ lCounter; return false; } );
	return lCounter;
}

bool const NNetModel::ConnectsTo( ShapeId const idSrc, ShapeId const idDst ) const
{
	if ( idSrc == idDst )
		return false;

	if ( isConnectedTo( idSrc, idDst ) )  // if already connected we cannot connect again
		return false;

	tShapeType const typeSrc { GetShapeType( idSrc ) };

	if ( IsPipelineType( typeSrc ) )          // only BaseKnots can be connected
		return false;                         // to other shapes

	if ( typeSrc == tShapeType::inputNeuron ) // input neuron cannot connect to anything
		return false;                         // try the other way round

	tShapeType const typeDst { GetShapeType( idDst ) };

	if ( IsPipelineType( typeDst ) )
	{
		if ( typeSrc == tShapeType::inputNeuron )   // cannot insert input neuron
			return false;                           // into pipeline
	}
	else if ( IsAnyNeuronType( typeDst ) )
	{
		if ( IsAnyNeuronType( typeSrc ) )  // neurons cannot connect directly 
			return false;                  // to other neurons

		if ( (typeDst == tShapeType::inputNeuron) && HasIncoming( idSrc ) )  // cannot connect incoming dendrite
			return false;                                                    // to input neuron

		if ( GetNrOfOutgoingConnections( idSrc ) + GetNrOfOutgoingConnections( idDst ) > 1 ) // neurons can not not have 
			return false;                                                                    // more than one axon
	}

	return true;
}

void NNetModel::RemoveShape( ShapeId const idShapeToBeDeleted )
{
	m_bUnsavedChanges = true;
	switch ( GetShapeType( idShapeToBeDeleted ) )
	{
	case tShapeType::pipeline:
		deletePipeline( idShapeToBeDeleted );
		break;

	case tShapeType::inputNeuron:
	case tShapeType::neuron:
		disconnectBaseKnot( GetTypedShape<BaseKnot>( idShapeToBeDeleted ) );
		deleteShape( idShapeToBeDeleted );
		break;

	case tShapeType::knot:
	default:
		break;
	}
	CHECK_CONSISTENCY;
}

void NNetModel::Disconnect( ShapeId const id )
{
	if ( BaseKnot * pBaseKnot { GetTypedShape<BaseKnot>( id ) } )
	{
		disconnectBaseKnot( pBaseKnot );
		assert( pBaseKnot->IsOrphan( ) );
		if ( pBaseKnot->GetShapeType( ) == tShapeType::knot )
			deleteShape( id );
	}
	CHECK_CONSISTENCY;
}

float const NNetModel::GetPulseRate( InputNeuron const * pInputNeuron ) const
{
	assert( pInputNeuron );
	return pInputNeuron->GetPulseFrequency().GetValue();
}

float const NNetModel::GetParameterValue( tParameter const param ) const
{
	switch ( param )
	{
		case tParameter::pulseSpeed:	 return m_pulseSpeed.GetValue();
		case tParameter::pulseWidth:	 return m_pulseWidth.GetValue();
		case tParameter::threshold:  	 return m_threshold.GetValue();
		case tParameter::peakVoltage:	 return m_peakVoltage.GetValue();
		case tParameter::refractPeriod:  return m_refractPeriod.GetValue();
		case tParameter::timeResolution: return m_usResolution.GetValue();
		case tParameter::signalLoss:     return 0.0f;  // not used, only for compaitibility reasons
		default: assert( false );
	}
	return 0.f;
}

void NNetModel::SetPulseRate( ShapeId const id, float const fNewValue )
{
	m_bUnsavedChanges = true;
	InputNeuron * const pInputNeuron { GetTypedShape<InputNeuron>( id ) };
	if ( pInputNeuron )
		pInputNeuron->SetPulseFrequency( static_cast< fHertz >( fNewValue ) );
}

void NNetModel::SetParameter
( 
	tParameter const param, 
	float      const fNewValue 
)
{
	m_bUnsavedChanges = true;
	switch ( param )
	{
		case tParameter::pulseSpeed:	 m_pulseSpeed    = static_cast< meterPerSec >( fNewValue ); break;
		case tParameter::pulseWidth:	 m_pulseWidth    = static_cast< MicroSecs >  ( fNewValue ); break;
		case tParameter::threshold:		 m_threshold     = static_cast< mV >         ( fNewValue ); break;
		case tParameter::peakVoltage:	 m_peakVoltage   = static_cast< mV >         ( fNewValue ); break;
		case tParameter::refractPeriod:	 m_refractPeriod = static_cast< MicroSecs >  ( fNewValue ); break;
		case tParameter::timeResolution: m_usResolution  = static_cast< MicroSecs >  ( fNewValue ); break;
		case tParameter::signalLoss: /* not used, only for compaitibility reasons */                break;
		default: assert( false );
	}
	RecalcAllShapes( );
	m_parameterObservable.NotifyAll( FALSE );
}

MicroMeterPoint const NNetModel::GetShapePos( ShapeId const id ) const 
{
	if ( BaseKnot const * pBaseKnot = GetConstTypedShape<BaseKnot>( id ) )
		return pBaseKnot->GetPosition();
	return NP_NULL;
}

tShapeType const NNetModel::GetShapeType( ShapeId const id ) const 
{
	if ( Shape const * pShape = GetConstShape( id ) )
		return pShape->GetShapeType();
	return tShapeType::undefined;
}

bool const NNetModel::HasOutgoing( ShapeId const id ) const 
{ 
	if ( BaseKnot const * pBaseKnot = GetConstTypedShape<BaseKnot>( id ) )
		return pBaseKnot->HasOutgoing( );
	return false;
}

bool const NNetModel::HasIncoming( ShapeId const id ) const 
{ 
	if ( BaseKnot const * pBaseKnot = GetConstTypedShape<BaseKnot>( id ) )
		return pBaseKnot->HasIncoming( );
	return false;
}

size_t const NNetModel::GetNrOfOutgoingConnections( ShapeId const id ) const 
{ 
	if ( BaseKnot const * pBaseKnot = GetConstTypedShape<BaseKnot>( id ) )
		return pBaseKnot->GetNrOfOutgoingConnections( );
	return false;
}

size_t const NNetModel::GetNrOfIncomingConnections( ShapeId const id ) const 
{ 
	if ( BaseKnot const * pBaseKnot = GetConstTypedShape<BaseKnot>( id ) )
		return pBaseKnot->GetNrOfIncomingConnections( );
	return false;
}

void NNetModel::Connect( ShapeId const idSrc, ShapeId const idDst )  // merge src shape into dst shape
{
	BaseKnot * pSrc = GetTypedShape<BaseKnot>( idSrc );
	if ( IsPipelineType( GetShapeType( idDst ) ) )   // connect baseknot to pipeline
	{
		insertBaseKnot( GetTypedShape<Pipeline>( idDst ), pSrc );
	}
	else  // connect baseknot to baseknot
	{
		BaseKnot * pDst = GetTypedShape<BaseKnot>( idDst );
		if ( pSrc && pDst )
		{
			pSrc->Apply2AllIncomingPipelines
			( 
				[&]( Pipeline * const pipe ) 
				{ 
					return connectIncoming( pipe, pDst );
				}
			);

			pSrc->Apply2AllOutgoingPipelines
			( 
				[&]( Pipeline * const pipe ) 
				{ 
					return connectOutgoing( pipe, pDst );
				}
			);

			if ( pSrc->GetShapeType() == tShapeType::knot )
				deleteShape( idSrc );
		}
	}
	m_bUnsavedChanges = true;
	CHECK_CONSISTENCY;
}

void NNetModel::NewPipeline( BaseKnot * const pStart, BaseKnot * const pEnd )
{
	if ( pStart && pEnd )
	{
		m_bUnsavedChanges = true;
		Pipeline * const pPipeline { NewShape<Pipeline>( NP_NULL ) };
		connectOutgoing( pPipeline, pStart );
		connectIncoming( pPipeline, pEnd );
		CHECK_CONSISTENCY;
	}
}

void NNetModel::MoveShape( ShapeId const id, MicroMeterPoint const & delta )
{
	m_bUnsavedChanges = true;
	switch ( GetShapeType( id ) )
	{
	case tShapeType::pipeline:
	{
		GetStartKnotPtr( id )->MoveShape( delta );
		GetEndKnotPtr  ( id )->MoveShape( delta );
		break;
	}

	case tShapeType::inputNeuron:
	case tShapeType::neuron:
	case tShapeType::knot:
		GetTypedShape<BaseKnot>( id )->MoveShape( delta );
		break;

	default:
		break;
	}
	CHECK_CONSISTENCY;
}

Neuron * const NNetModel::InsertNeuron( ShapeId const idPipeline, MicroMeterPoint const & splitPoint )
{
	Neuron * pNeuron { nullptr };
	if ( IsDefined( idPipeline ) )
	{
		m_bUnsavedChanges = true;
		pNeuron = NewShape<Neuron>( splitPoint );
		insertBaseKnot( GetTypedShape<Pipeline>(idPipeline), pNeuron );
		CHECK_CONSISTENCY;
	}
	return pNeuron;
}

Knot * const NNetModel::InsertKnot( ShapeId const idPipeline, MicroMeterPoint const & splitPoint )
{
	Knot * pKnot { nullptr };
	if ( IsDefined( idPipeline ) )
	{
		m_bUnsavedChanges = true;
		pKnot = NewShape<Knot>( splitPoint );
		insertBaseKnot( GetTypedShape<Pipeline>(idPipeline), pKnot );
		CHECK_CONSISTENCY;
	}
	return pKnot;
}

void NNetModel::AddOutgoing2Pipe( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsDefined( id ) )
		NewPipeline( InsertKnot( id, pos ), NewShape<Knot>( pos + orthoVector( id ) ) );
}

void NNetModel::AddIncoming2Pipe( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsDefined( id ) )
		NewPipeline( NewShape<Knot>( pos - orthoVector( id ) ), InsertKnot( id, pos ) );
}

void NNetModel::AddOutgoing2Knot( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsDefined( id ) )
		NewPipeline( GetTypedShape<BaseKnot>(id), NewShape<Knot>( pos + STD_OFFSET) );
}

void NNetModel::AddIncoming2Knot( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsDefined( id ) )
		NewPipeline( NewShape<Knot>( pos - STD_OFFSET ), GetTypedShape<BaseKnot>( id ) );
}

void NNetModel::Compute( )
{
	Apply2All<Shape>( [&]( Shape & shape ) { shape.Prepare( ); return false; } );
	Apply2All<Shape>( [&]( Shape & shape ) { shape.Step   ( ); return false; } );

	m_timeStamp += GetTimeResolution( );
}

void NNetModel::ResetModel( )
{
	EnterCritSect( );
	Apply2All<Shape>( [&]( Shape & shape ) { delete & shape; return false; } );
	m_Shapes.clear();
	LeaveCritSect( );
}

Shape const * NNetModel::FindShapeAt( MicroMeterPoint const pnt, function<bool(Shape const &)> const & crit ) const
{	
	Shape const * pRes { nullptr };

	EnterCritSect( );

	if ( ! pRes )  // first test all neurons and input neurons
		pRes = findShapeAt( pnt, [&]( Shape const & s ) { return s.IsAnyNeuron( ) && crit( s ); } );

	if ( ! pRes )   // if nothing found, test knot shapes
		pRes = findShapeAt( pnt, [&]( Shape const & s ) { return s.IsKnot     ( ) && crit( s ); } ); 	

	if ( ! pRes ) // if nothing found, try pipelines
		pRes = findShapeAt( pnt, [&]( Shape const & s ) { return s.IsPipeline ( ) && crit( s ); } );

	LeaveCritSect( );

	return pRes;
}

COLORREF const NNetModel::GetFrameColor( tHighlightType const type ) const 
{ 
	return ( type == tHighlightType::superHighlighted )
		? EXT_COLOR_SUPER_HIGHLIGHT 
		: ( type == tHighlightType::highlighted )
		? EXT_COLOR_HIGHLIGHT 
		: EXT_COLOR_NORMAL;
};

/////////////////// local functions ///////////////////////////////////////////////

void NNetModel::checkConsistency( Shape const * pShape ) const
{
	tShapeType type = pShape->GetShapeType();

	if ( ::IsBaseKnotType( type ) )
		static_cast<BaseKnot const &>( * pShape ).Apply2AllConnectedPipelinesConst
		( 
			[&]( Pipeline const * const pipe ) 
			{ 
				assert( pipe->IsPipeline() ); 
				return false; 
			} 
	    );

	switch ( type )
	{
	case tShapeType::inputNeuron:
		assert( ! static_cast<InputNeuron const *>( pShape )->HasIncoming() );
		break;

	case tShapeType::knot:
		break;

	case tShapeType::neuron:
		break;

	case tShapeType::pipeline:
		{
			Pipeline const * pPipeline { static_cast<Pipeline const *>( pShape ) };
			assert( pPipeline );
			if ( Shape const * const pStart { pPipeline->GetStartKnotPtr() } )
				assert( IsBaseKnotType( pStart->GetShapeType() ) );
			if ( Shape const * const pEnd   { pPipeline->GetEndKnotPtr() } )
				assert( IsBaseKnotType( pEnd->GetShapeType() ) );
			break;
		}

	case tShapeType::undefined:
		assert( false );
		break;

	default:
		assert( false );
	}
}

bool const NNetModel::isConnectedToPipeline( ShapeId const id, Pipeline const * const pPipeline ) const
{
	return (id == pPipeline->GetStartKnotId()) || (id == pPipeline->GetEndKnotId());
}

bool const NNetModel::isConnectedTo( ShapeId id1, ShapeId id2 ) const
{
	if ( IsType<Pipeline>( id1 ) )
		return isConnectedToPipeline( id2, GetConstTypedShape<Pipeline>( id1 ) );
	if ( IsType<Pipeline>( id2 ) )
		return isConnectedToPipeline( id1, GetConstTypedShape<Pipeline>( id2 ) );
	else
		return false;
}

void NNetModel::deleteShape( ShapeId const id )
{
	if ( IsDefined( id ) )
	{
		EnterCritSect( );
		delete m_Shapes[ id.GetValue() ];
		m_Shapes[ id.GetValue() ] = nullptr;
		LeaveCritSect( );
	}
}

bool NNetModel::connectIncoming
( 
	Pipeline * const pPipeline, 
	BaseKnot * const pEndPoint
)
{
	if ( pPipeline && pEndPoint )
	{
		pEndPoint->AddIncoming( pPipeline );
		pPipeline->SetEndKnot ( pEndPoint );
	}
	return false;
}

bool NNetModel::connectOutgoing
( 
	Pipeline * const pPipeline, 
	BaseKnot * const pStartPoint
)
{
	if ( pPipeline && pStartPoint )
	{
		pStartPoint->AddOutgoing( pPipeline );
		pPipeline->SetStartKnot ( pStartPoint );
	}
	return false;
}

void NNetModel::disconnectBaseKnot( BaseKnot * const pBaseKnot ) // disconnects only, shape remains
{
	if ( pBaseKnot )
	{
		MicroMeterPoint umPos { pBaseKnot->GetPosition() };
		pBaseKnot->Apply2AllIncomingPipelines
		( 
			[&]( Pipeline * const pipe ) // every incoming pipeline needs a new end knot
			{ 
				connectIncoming( pipe, NewShape<Knot>( umPos ) );
				pipe->DislocateEndPoint( );
				return false;
			} 
		);
		pBaseKnot->ClearIncoming();
		pBaseKnot->Apply2AllOutgoingPipelines
		( 
			[&]( Pipeline * const pipe ) // every outgoing pipeline needs a new start knot
			{ 
				connectOutgoing( pipe, NewShape<Knot>( umPos ) );
				pipe->DislocateStartPoint( );
				return false;
			} 
		);
		pBaseKnot->ClearOutgoing();
	}
}

void NNetModel::deletePipeline( ShapeId const id )
{
	if ( IsDefined( id ) )
	{
		Pipeline * pPipelineToBeDeleted { GetTypedShape<Pipeline>( id ) };

		BaseKnot * pStartKnot  { pPipelineToBeDeleted->GetStartKnotPtr() };
		pStartKnot->RemoveOutgoing( pPipelineToBeDeleted );
		if ( pStartKnot->IsOrphanedKnot( ) && ( pStartKnot->GetShapeType( ) == tShapeType::knot ) )
			deleteShape( pStartKnot->GetId() );

		BaseKnot * pEndKnot   { pPipelineToBeDeleted->GetEndKnotPtr() };
		pEndKnot->RemoveIncoming( pPipelineToBeDeleted );
		if ( pEndKnot->IsOrphanedKnot( ) && ( pEndKnot->GetShapeType( ) == tShapeType::knot ) )
			deleteShape( pEndKnot->GetId() );

		deleteShape( id );
	}
}

void NNetModel::insertBaseKnot( Pipeline * const pPipeline, BaseKnot * const pBaseKnot)
{
	if ( pPipeline && pBaseKnot )
	{
		BaseKnot * const pStartKnot { pPipeline->GetStartKnotPtr( ) };
		NewPipeline( pStartKnot, pBaseKnot );
		pStartKnot->RemoveOutgoing( pPipeline );
		connectOutgoing( pPipeline, pBaseKnot );
	}
}

MicroMeterPoint NNetModel::orthoVector( ShapeId const idPipeline ) const
{
	Pipeline const * pPipeline { GetConstTypedShape<Pipeline>( idPipeline ) };
	return OrthoVector( pPipeline->GetVector(), NEURON_RADIUS * 2.f );
}

Shape const * NNetModel::findShapeAt( MicroMeterPoint const pnt, function<bool(Shape const &)> const & crit ) const
{
	for ( size_t i = m_Shapes.size(); i --> 0; )	
	{
		Shape * pShape = m_Shapes[i];
		if ( pShape && crit( * pShape ) && pShape->IsPointInShape( pnt ) ) 
			return pShape;
	};
	return nullptr;
}