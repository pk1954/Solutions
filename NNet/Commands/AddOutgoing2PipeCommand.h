// AddOutgoing2PipeCommand.h
//
// Commands

#pragma once

#include "MoreTypes.h"
#include "NNetModelWriterInterface.h"
#include "ShapeId.h"
#include "Command.h"
#include "BaseKnot.h"

class AddOutgoing2PipeCommand : public Command
{
public:
	AddOutgoing2PipeCommand
	( 
		ShapeId         const   idPipe, 
		MicroMeterPoint const & pos 
	)
	  :	m_idPipe(idPipe),
		m_pos(pos)
	{ }

	~AddOutgoing2PipeCommand()	{}

	virtual void Do( NNetModelWriterInterface & nmwi ) 
	{ 
		if ( ! m_upKnotInsert )
		{
			m_pPipeOld      = nmwi.GetShapePtr<Pipe *>( m_idPipe );
			m_pStartKnotOld = m_pPipeOld->GetStartKnotPtr();
			m_upKnotInsert  = make_unique<Knot>( m_pos );
			m_upKnotInsert->Select( m_pPipeOld->IsSelected(), false );
			m_upKnotOrtho   = make_unique<Knot>( m_pos + nmwi.OrthoVector( m_idPipe ) );
			m_upPipeOrtho   = make_unique<Pipe>( m_upKnotInsert.get(), m_upKnotOrtho.get() );
			m_upPipeExt     = make_unique<Pipe>( m_pStartKnotOld, m_upKnotInsert.get() );	

			m_upKnotInsert->m_connections.AddOutgoing( m_upPipeOrtho.get() );
			m_upKnotOrtho ->m_connections.AddIncoming( m_upPipeOrtho.get() );
			m_upKnotInsert->m_connections.AddIncoming( m_upPipeExt.get() );
			m_upKnotInsert->m_connections.AddOutgoing( m_pPipeOld );
		}
		m_pStartKnotOld->m_connections.ReplaceOutgoing( m_pPipeOld, m_upPipeExt.get() );
		m_pPipeOld->SetStartKnot( m_upKnotInsert.get() );
		nmwi.GetUPShapes().Push( move(m_upKnotOrtho ) );
		nmwi.GetUPShapes().Push( move(m_upKnotInsert) );
		nmwi.GetUPShapes().Push( move(m_upPipeOrtho ) );
		nmwi.GetUPShapes().Push( move(m_upPipeExt   ) );
	}

	virtual void Undo( NNetModelWriterInterface & nmwi ) 
	{ 
		m_upPipeExt    = nmwi.GetUPShapes().Pop<Pipe>();
		m_upPipeOrtho  = nmwi.GetUPShapes().Pop<Pipe>();
		m_upKnotInsert = nmwi.GetUPShapes().Pop<Knot>();
		m_upKnotOrtho  = nmwi.GetUPShapes().Pop<Knot>();
		m_pPipeOld->SetStartKnot( m_pStartKnotOld );
		m_pStartKnotOld->m_connections.ReplaceOutgoing( m_upPipeExt.get(), m_pPipeOld );
	}

private:
	Pipe     *       m_pPipeOld      { nullptr };
	BaseKnot *       m_pStartKnotOld { nullptr };
	unique_ptr<Pipe> m_upPipeExt     { nullptr };
	unique_ptr<Pipe> m_upPipeOrtho   { nullptr };
	unique_ptr<Knot> m_upKnotInsert  { nullptr };
	unique_ptr<Knot> m_upKnotOrtho   { nullptr }; 
	ShapeId          const m_idPipe;
	MicroMeterPoint  const m_pos; 
};
