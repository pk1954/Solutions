// BaseKnot.h
//
// NNetModel
//
// geometry aspects of all kinds of knots

#pragma once

#include <vector>
#include "PixelTypes.h"
#include "MoreTypes.h"
#include "Shape.h"

using std::vector;
using std::wstring;

class GraphicsInterface;
class PixelCoordsFp;
class NNetModel;
class Pipeline;

struct IDWriteTextFormat;

class BaseKnot : public Shape
{
public:
	BaseKnot
	( 
		NNetModel     * const pModel,
		MicroMeterPoint const center,
		tShapeType      const type,
		MicroMeter      const extension
 	)
	  : Shape( pModel, type ),
		m_center( center ),
		m_extension( extension ),
		m_pTextFormat( nullptr )
	{ }

	virtual ~BaseKnot() {}

	virtual mV GetNextOutput( ) const = 0;

	static bool TypeFits( tShapeType const type ) { return IsBaseKnotType( type ); }

	MicroMeterPoint GetPosition( )  const { return m_center;	}
	MicroMeter      GetExtension( ) const { return m_extension;	}

	bool IsPointInShape( MicroMeterPoint const & ) const;

	void AddIncoming    ( Pipeline * const );
	void AddOutgoing    ( Pipeline * const );
	void RemoveIncoming ( Pipeline * const );
	void RemoveOutgoing ( Pipeline * const );
	void ReplaceIncoming( Pipeline * const, Pipeline * const );
	void ReplaceOutgoing( Pipeline * const, Pipeline * const );

	bool   HasIncoming( )                const { return ! m_incoming.empty(); }
	bool   HasOutgoing( )                const { return ! m_outgoing.empty(); }
	size_t GetNrOfIncomingConnections( ) const { return m_incoming.size(); }
	size_t GetNrOfOutgoingConnections( ) const { return m_outgoing.size(); }
	size_t GetNrOfConnections( )         const { return m_incoming.size() + m_outgoing.size(); }
	bool   IsOrphan( )                   const { return m_incoming.empty() && m_outgoing.empty(); }
	bool   IsOrphanedKnot( )             const { return (GetShapeType() == tShapeType::knot) && IsOrphan(); }

	void ClearIncoming( ) { m_incoming.clear(); }
	void ClearOutgoing( ) { m_outgoing.clear(); }

	bool IsPrecursorOf( ShapeId const );
	bool IsSuccessorOf( ShapeId const );

	Pipeline * const GetAxon( ) const
	{
		assert( m_outgoing.size() == 1 );
		return m_outgoing.front();
	}

	void Apply2AllIncomingPipelines( function<void(Pipeline * const)> const & func )
	{
		for ( auto & pipe : m_incoming )
		{
			if ( pipe )
				func( pipe );
		}
	}

	void Apply2AllOutgoingPipelines( function<void(Pipeline * const)> const & func )
	{
		for ( auto & pipe : m_outgoing )
		{
			if ( pipe )
				func( pipe );
		}
	}

	void Apply2AllIncomingPipelinesConst( function<void(Pipeline const * const)> const & func ) const
	{
		for ( auto pipe : m_incoming )
		{
			if ( pipe )
				func( pipe );
		}
	}

	void Apply2AllOutgoingPipelinesConst( function<void(Pipeline const * const)> const & func ) const
	{
		for ( auto pipe : m_outgoing )
		{
			if ( pipe )
				func( pipe );
		}
	}

	void Apply2AllConnectedPipelines( function<void(Pipeline const *)> const & func )
	{
		Apply2AllIncomingPipelines( [&]( Pipeline const * pipe ) { func( pipe ); } );
		Apply2AllOutgoingPipelines( [&]( Pipeline const * pipe ) { func( pipe ); } );
	}

	void Apply2AllConnectedPipelinesConst( function<void(Pipeline const * const)> const & func ) const
	{
		Apply2AllIncomingPipelinesConst( [&]( Pipeline const * const pipe ) { func( pipe ); } );
		Apply2AllOutgoingPipelinesConst( [&]( Pipeline const * const pipe ) { func( pipe ); } );
	}

	virtual void MoveShape( MicroMeterPoint const & );

	virtual void DrawNeuronText( PixelCoordsFp & ) const;

protected:

	vector<Pipeline *> m_incoming;
	vector<Pipeline *> m_outgoing;

	void drawCircle( PixelCoordsFp const &, COLORREF const, MicroMeterPoint const, MicroMeter const ) const;
	void drawCircle( PixelCoordsFp const &, COLORREF const, MicroMeter const ) const;

	PixelRect const GetPixRect4Text( PixelCoordsFp const & ) const;
	void      const DisplayText( PixelRect const, wstring const ) const;

private:

	MicroMeterPoint     m_center;
	MicroMeter          m_extension;
	IDWriteTextFormat * m_pTextFormat;
};
