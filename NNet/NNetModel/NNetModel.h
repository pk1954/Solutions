// NNetModel.h 
//
// NNetModel

#pragma once

#include <vector>
#include "util.h"
#include "BoolOp.h"
#include "MoreTypes.h"
#include "ObserverInterface.h"
#include "Segment.h"
#include "tParameter.h"
#include "tHighlightType.h"
#include "InputNeuron.h"
#include "Neuron.h"
#include "Knot.h"
#include "Pipe.h"

class ObserverInterface;
class EventInterface;
class ComputeThread;
class Param;

using ShapeList = vector<Shape *>;

// non member function used in class definition 
MicroMeterRect GetEnclosingRect( ShapeList const & );

class NNetModel
{
public:

	NNetModel( Param * const, ObserverInterface * const );

	virtual ~NNetModel( );

	// readOnly functions

	template <typename T> bool HasType( Shape const * const pShape ) const 
	{ 
		return std::remove_pointer<T>::type::TypeFits( pShape->GetShapeType() ); 
	}

	template <typename T>
	T GetShapePtr( ShapeId const id ) 
	{
		Shape * const pShape { GetShape( id ) };
		return (pShape && HasType<T>(pShape)) ? static_cast<T>( pShape ) : nullptr;
	}

	template <typename T>
	T GetShapeConstPtr( ShapeId const id ) const
	{
		Shape const * const pShape { GetConstShape( id ) };
		return (pShape && HasType<T>(pShape)) ? static_cast<T>( pShape ) : nullptr;
	}

	template <typename T>
	long const GetNrOf( ) const
	{
		long lCounter = 0;
		Apply2All<T>( [&]( T const & s ) { ++ lCounter; } );
		return lCounter;
	}

	bool          IsShapeIdOk  ( ShapeId const id ) const { return IsDefined( id ) && IsValidShapeId( id ); }
	Shape       * GetShape     ( ShapeId const id )       { return IsShapeIdOk( id ) ? m_Shapes[id.GetValue()] : nullptr; }
	Shape const * GetConstShape( ShapeId const id ) const {	return IsShapeIdOk( id ) ? m_Shapes[id.GetValue()] : nullptr; }
	
	MicroMeterPoint const GetShapePos   ( ShapeId const id  ) const;
	bool            const IsValidShapeId( ShapeId const id  ) const { return id.GetValue() < m_Shapes.size(); }

	fMicroSecs      const GetSimulationTime ( ) const { return m_timeStamp; }
	long            const GetSizeOfShapeList( ) const { return CastToLong( m_Shapes.size() ); }
	

	BaseKnot * const GetStartKnotPtr(ShapeId const id) const { return GetShapeConstPtr<Pipe const *>(id)->GetStartKnotPtr(); }
	BaseKnot * const GetEndKnotPtr  (ShapeId const id) const { return GetShapeConstPtr<Pipe const *>(id)->GetEndKnotPtr  (); }

	ShapeId const GetStartKnotId (ShapeId const idPipe) const { return GetStartKnotPtr(idPipe)->GetId(); }
	ShapeId const GetEndKnotId   (ShapeId const idPipe) const { return GetEndKnotPtr  (idPipe)->GetId(); }

	// manipulating functions

	void ResetSimulationTime( )	{ m_timeStamp = 0._MicroSecs; }

	template <typename T> 
	T * const NewShape( MicroMeterPoint const & pos ) 
	{ 
		auto pT { new T( pos ) };
		add2ShapeList( pT );
		modelHasChanged( );
		return pT;
	}

	template <typename T>
	void AppendShape( ShapeId const id )
	{
		Shape * pShape { GetShapePtr<Shape *>( id ) };
		if ( pShape && pShape->TypeFits( ShapeType::Value::knot ) )
		{
			Connect( id, NewShape<T>( GetShapePos( id ) )->GetId() );
			modelHasChanged( );
		}
	}

	template <typename T>
	bool Apply2AllB( function<bool(T &)> const & func ) const
	{
		bool bResult { false };
		for ( auto pShape : m_Shapes )
		{
			if ( pShape )
			{
				pShape->LockShapeExclusive();
				if ( HasType<T>( pShape ) )	bResult = func( static_cast<T &>( * pShape ) );
				pShape->UnlockShapeExclusive();
				if ( bResult )
					break;
			}
		}
		return bResult;
	}

	template <typename T>   // const version
	bool Apply2AllB( function<bool(T const &)> const & func ) const
	{
		bool bResult { false };
		for ( auto pShape : m_Shapes )
		{
			if ( pShape )
			{
				pShape->LockShapeShared();
				if ( HasType<T>( pShape ) )	
					bResult = func( static_cast<T const &>( * pShape ) );
				pShape->UnlockShapeShared();
				if ( bResult )
					break;
			}
		}
		return bResult;
	}

	template <typename T>
	void Apply2All( function<void(T &)> const & func ) const
	{
		for (auto & pShape : m_Shapes)    
		{ 
			if ( pShape )
			{
				pShape->LockShapeExclusive();
				if ( HasType<T>(pShape) ) func( static_cast<T &>( * pShape) ); 
				pShape->UnlockShapeExclusive();
			}
		}
	}                        

	template <typename T>    // const version
	void Apply2All( function<void(T const &)> const & func ) const
	{
		for (auto & pShape : m_Shapes)    
		{ 
			if ( pShape )
			{
				pShape->LockShapeShared();
				if ( HasType<T>(pShape) ) 
					func( static_cast<T const &>( * pShape) ); 
				pShape->UnlockShapeShared();
			}
		}
	}                        

	template <typename T>
	void Apply2AllInRect( MicroMeterRect const & r, function<void(T       &)> const & func )
	{
		Apply2All<T>( [&](T & s) { if ( s.IsInRect(r) ) { func( s ); } } );
	}

	template <typename T>   // const version
	void Apply2AllInRect( MicroMeterRect const & r, function<void(T const &)> const & func )
	{
		Apply2All<T>( [&](T & const s) { if ( s.IsInRect(r) ) { func( s ); } } );
	}

	template <typename T>
	void Apply2AllSelected( function<void(T &)> const & func ) const
	{
		Apply2All<T>( {	[&](T & s) { if ( s.IsSelected() ) { func( s ); } } } );
	}

	template <typename T>  // const version
	void Apply2AllSelected( function<void(T const &)> const & func ) const
	{
		Apply2All<T>( {	[&](T const & s) { if ( s.IsSelected() ) { func( s ); } } } );
	}

	virtual void Compute( );

	void CreateInitialShapes();
	void SetShape( Shape * const pShape, ShapeId const id )	{ m_Shapes[ id.GetValue() ] = pShape; }
	void NewPipe( BaseKnot * const, BaseKnot * const );

	Knot   * const InsertKnot  ( ShapeId const, MicroMeterPoint const & );
	Neuron * const InsertNeuron( ShapeId const, MicroMeterPoint const & );
	void           MoveShape   ( ShapeId const, MicroMeterPoint const & );

	ShapeId const FindShapeAt( MicroMeterPoint const &, ShapeCrit const & ) const;

	void AddOutgoing2Knot( ShapeId const, MicroMeterPoint const & );
	void AddIncoming2Knot( ShapeId const, MicroMeterPoint const & );
	void AddOutgoing2Pipe( ShapeId const, MicroMeterPoint const & );
	void AddIncoming2Pipe( ShapeId const, MicroMeterPoint const & );

	void Connect( ShapeId const, ShapeId const );
	void Convert2Neuron( ShapeId const );
	void Convert2InputNeuron( ShapeId const );
	void Disconnect( ShapeId const );
	void RecalcAllShapes( );
	void ResetModel( );
	void SetPulseRate_Lock   ( ShapeId const, float const );
	void SetParameter( tParameter const, float const );
	void SetNrOfShapes( long lNrOfShapes ) { m_Shapes.resize( lNrOfShapes ); }
	void SetTriggerSound( ShapeId const, Hertz const, MilliSecs const );
	void StopTriggerSound( ShapeId const id ) {	SetTriggerSound( id, 0_Hertz, 0_MilliSecs ); }

	void RemoveShape( ShapeId const id ) 
	{ 
		removeShape( GetShape( id ) ); 
		modelHasChanged( );
	}

	MicroMeterRect GetEnclosingRect() const { return ::GetEnclosingRect( m_Shapes ); }

	void ClearModel()                { Apply2All<Shape>( [&](Shape &s) { s.Clear( ); } ); }
	void SelectAll(tBoolOp const op) { Apply2All<Shape>( [&](Shape &s) { s.Select( op ); } ); }

	void ConnectIncoming( Pipe * const, BaseKnot * const );
	void ConnectOutgoing( Pipe * const, BaseKnot * const );
	void CopySelection( );
	void MarkSelection( tBoolOp const );
	void DeleteSelection( );
	void MoveSelection( MicroMeterPoint const & );
	void SelectBeepers( ) {	Apply2All<Neuron>( [&](Neuron & n) { if (n.HasTriggerSound()) n.Select( tBoolOp::opTrue ); } ); }

	bool const AnyShapesSelected( ) const
	{
		return Apply2AllB<Shape>( [&]( Shape const & shape ) { return shape.IsSelected(); } );
	}

	void RemoveBeepers( ) 
	{	
		if ( AnyShapesSelected() )
			Apply2AllSelected<Neuron>( [&](Neuron & n) { removeTriggerSound( & n ); } );
		else
			Apply2All        <Neuron>( [&](Neuron & n) { removeTriggerSound( & n ); } );
	}

	void SelectSubtree( ShapeId const idBaseKnot, tBoolOp const op )
	{
		selectSubtree( GetShapePtr<BaseKnot *>( idBaseKnot ), op );
	}

	void SelectShape( ShapeId const idShape, tBoolOp const op )
	{
		if ( Shape * const pShape { GetShapePtr<Shape *>( idShape ) } )
			pShape->Select( op );
	}

	void SelectShapesInRect( MicroMeterRect const & umRect )
	{
		Apply2AllInRect<Shape>( umRect, [&]( Shape & shape ) { shape.Select( tBoolOp::opTrue ); } );
	}

	void MarkShape( ShapeId const idShape, tBoolOp const op )
	{
		if ( Shape * const pShape { GetShapePtr<Shape *>( idShape ) } )
			pShape->Mark( op );
	}

	bool IsSelected( ShapeId const idShape )
	{
		if ( Shape const * pShape { GetShapePtr<Shape *>( idShape ) } )
			return pShape->IsSelected();
		else 
			return false;
	}

	void LockModelShared() const
	{ 
		AcquireSRWLockShared( & m_SRWLockModel );
	}

	void UnlockModelShared() const
	{ 
		ReleaseSRWLockShared( & m_SRWLockModel );
	}

private:

	mutable SRWLOCK     m_SRWLockModel    { SRWLOCK_INIT }; // locks model during deletion or reconstruction  

	ShapeList           m_Shapes          { };
	fMicroSecs          m_timeStamp       { 0._MicroSecs };
	Param             * m_pParam          { nullptr };
	ObserverInterface * m_pChangeObserver { nullptr };

	unsigned long m_nrOfShapes       { 0L };
	unsigned long m_nrOfPipes        { 0L };
	unsigned long m_nrOfKnots        { 0L };
	unsigned long m_nrOfNeurons      { 0L };
	unsigned long m_nrOfINputNeurons { 0L };

	// local functions

	void add2ShapeList( Shape * const pNewShape )
	{
		pNewShape->SetId( ShapeId { GetSizeOfShapeList() } );
		m_Shapes.push_back( pNewShape );
	}

	MicroMeterPoint orthoVector        ( ShapeId const ) const;
	void            deletePipeEndPoints( Pipe  * const );
	void            deleteShape        ( Shape * const );
	void            removeShape        ( Shape * const );
	Shape *         shallowCopy        ( Shape   const & );
	void            disconnectBaseKnot ( BaseKnot * const );
	void            selectSubtree      ( BaseKnot * const, tBoolOp    const );
	void            insertBaseKnot     ( Pipe     * const, BaseKnot * const );
	void            connectToNewShapes ( Shape const &, ShapeList & );
	void            modelHasChanged    ( ) const;
	void            setTriggerSound    ( Neuron * const, Hertz const, MilliSecs const );
	void            removeTriggerSound ( Neuron * const );
	ShapeId const   findShapeAt        ( MicroMeterPoint const, ShapeCrit const & ) const;
};
