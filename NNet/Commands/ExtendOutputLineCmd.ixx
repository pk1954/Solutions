// ExtendOutputLineCmd.ixx
//
// Commands

module;

#include <memory>

export module ExtendOutputLineCmd;

import Types;
import NNetCommand;
import NNetModel;

using std::make_unique;
using std::unique_ptr;

export class ExtendOutputLineCmd : public NNetCommand
{
public:
	ExtendOutputLineCmd      // case 11: Extend Outputline, adding a new knot       
	(                               
		NobId         const   id,   
		MicroMeterPnt const & pos    
	)
      :	m_outputLineOld(*m_pNMWI->GetNobPtr<OutputLine*>(id))
	{
		m_upPipe          = make_unique<Pipe>();
		m_upKnotNew       = make_unique<Knot>(m_outputLineOld.GetPos());
		m_upOutputLineNew = make_unique<OutputLine>(pos);
		ConnectIncoming(*m_upPipe.get(), *m_upOutputLineNew.get());
		ConnectOutgoing(*m_upPipe.get(), *m_upKnotNew.get());
		m_upKnotNew->SetIncoming(m_outputLineOld);
	}

	~ExtendOutputLineCmd() final = default;

	void Do() final 
	{ 
		m_outputLineOld.Apply2AllInPipes([this](Pipe &p){ ConnectIncoming(p, *m_upKnotNew.get()); });
		m_pNMWI->Push2Model(move(m_upKnotNew));
		m_pNMWI->Push2Model(move(m_upPipe));
		m_pNMWI->Push2Model(move(m_upOutputLineNew));
		m_upOutputLineOld = m_pNMWI->RemoveFromModel<OutputLine>(m_outputLineOld);
	}

	void Undo() final 
	{ 
		m_upOutputLineNew = m_pNMWI->PopFromModel<OutputLine>();
		m_upPipe          = m_pNMWI->PopFromModel<Pipe>();
		m_upKnotNew       = m_pNMWI->PopFromModel<Knot>();
		m_outputLineOld.Apply2AllInPipes([this](Pipe &p){ ConnectIncoming(p, *m_upOutputLineOld.get()); });
		m_pNMWI->Restore2Model(move(m_upOutputLineOld)); 
	}

private:

	BaseKnot       const & m_outputLineOld;
	unique_ptr<OutputLine> m_upOutputLineOld;
	unique_ptr<Knot>       m_upKnotNew;
	unique_ptr<OutputLine> m_upOutputLineNew;
	unique_ptr<Pipe>       m_upPipe;
};