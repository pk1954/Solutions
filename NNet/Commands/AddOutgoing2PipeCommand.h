// AddOutgoing2PipeCommand.h
//
// Commands

#pragma once

#include "MoreTypes.h"
#include "NNetModelWriterInterface.h"
#include "NobId.h"
#include "Command.h"
#include "BaseKnot.h"

class AddOutgoing2PipeCommand : public Command
{
public:
	AddOutgoing2PipeCommand
	( 
		NobId         const   idPipe, 
		MicroMeterPnt const & pos 
	)
	  :	m_idPipe(idPipe),
		m_pos(pos)
	{ }

	~AddOutgoing2PipeCommand()	{}

	virtual void Do( NNetModelWriterInterface & nmwi ) 
	{ 
		if ( ! m_upKnotInsert )
		{
			m_pPipeOld      = nmwi.GetNobPtr<Pipe *>( m_idPipe );
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
		nmwi.Add2Model( move(m_upKnotOrtho ) );
		nmwi.Add2Model( move(m_upKnotInsert) );
		nmwi.Add2Model( move(m_upPipeOrtho ) );
		nmwi.Add2Model( move(m_upPipeExt   ) );
	}

	virtual void Undo( NNetModelWriterInterface & nmwi ) 
	{ 
		m_upPipeExt    = nmwi.GetUPNobs().Pop<Pipe>();
		m_upPipeOrtho  = nmwi.GetUPNobs().Pop<Pipe>();
		m_upKnotInsert = nmwi.GetUPNobs().Pop<Knot>();
		m_upKnotOrtho  = nmwi.GetUPNobs().Pop<Knot>();
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
	NobId          const m_idPipe;
	MicroMeterPnt  const m_pos; 
};
