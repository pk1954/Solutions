// AttachSigGen2ConCmd.ixx
//
// Commands

module;

#include <iostream>

export module AttachSigGen2ConCmd;

import NNetCommand;
import NNetModel;

export class AttachSigGen2ConCmd : public NNetCommand
{
public:
	explicit AttachSigGen2ConCmd(NobId const nobId)
	  : m_inputConnector(* m_pNMWI->GetNobPtr<InputConnector *>(nobId)),
		m_pSigGenOld(m_inputConnector.GetSigGen())
	{}

	void Do() final
	{
		m_inputConnector.SetSigGen(m_pSigGenNew);
	}

	void Undo() final
	{
		m_inputConnector.SetSigGen(m_pSigGenOld);
	}

	static void Register()
	{
		SymbolTable::ScrDefConst(NAME, new Wrapper);
	}

	static void Push(NobId const id)
	{
		if (IsTraceOn())
			TraceStream() << NAME << id.GetValue() << endl;
		m_pStack->PushCommand(make_unique<AttachSigGen2ConCmd>(id));
	}

private:

	inline static const wstring NAME { L"AttachSigGen2Con" };

	class Wrapper : public ScriptFunctor
	{
	public:
		void operator() (Script& script) const final
		{
			AttachSigGen2ConCmd::Push(ScrReadNobId(script));
		}
	};

	InputConnector  & m_inputConnector;
	SignalGenerator * m_pSigGenOld;
	SignalGenerator * m_pSigGenNew { m_pNMWI->GetSigGenSelected() };
};