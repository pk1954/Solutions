// SelectAllConnectedCmd.ixx
//
// Commands

module;

#include <iostream>

export module SelectAllConnectedCmd;

import SelectionCommand;
import UtilityWrappers;

export class SelectAllConnectedCmd : public SelectionCommand
{
public:
	SelectAllConnectedCmd(NobId const idNob)
	  : m_nobStart(*m_pNMWI->GetNobPtr<Nob *>(idNob))
	{}

	void Do() final
	{
		SelectionCommand::Do();
		m_nobStart.SelectAllConnected(true);
	}

	static void Register()
	{
		SymbolTable::ScrDefConst(NAME, new Wrapper);
	}

	static void Push(NobId const idNob)
	{
		if (IsTraceOn())
			TraceStream() << NAME << L' ' << idNob << endl;
		m_pStack->PushCommand(make_unique<SelectAllConnectedCmd>(idNob));
	}

private:

	inline static const wstring NAME { L"SelectAllConnected" };

	class Wrapper : public ScriptFunctor
	{
	public:
		void operator() (Script& script) const final
		{
			SelectAllConnectedCmd::Push(ScrReadNobId(script));
		}
	};

	Nob& m_nobStart;
	bool m_bOn;
};
