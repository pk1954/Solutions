// InsertKnotCommand.ixx
//
// Commands

module;

#include <iostream>

export module InsertKnotCommand;

import SplitPipeCommand;

export class InsertKnotCommand : public SplitPipeCommand
{
public:
	InsertKnotCommand
	(
		NobId         const  idPipe,
		MicroMeterPnt const& umSplitPoint
	)
		: SplitPipeCommand(idPipe)
	{
		m_upInsertKnot = make_unique<Knot>(umSplitPoint);
		SplitPipeCommand::InitSplit(*m_upInsertKnot.get());
	}

	~InsertKnotCommand() final = default;

	void Do() final
	{
		SplitPipeCommand::Do();
		m_pNMWI->Push2Model(move(m_upInsertKnot));
	}

	void Undo() final
	{
		m_upInsertKnot = m_pNMWI->PopFromModel<Knot>();
		SplitPipeCommand::Undo();
	}

	static void Register()
	{
		SymbolTable::ScrDefConst(NAME, new Wrapper);
	}

	static void Push
	(
		NobId         const  idPipe,
		MicroMeterPnt const& umSplitPoint
	)
	{
		if (IsTraceOn())
			TraceStream() << NAME << idPipe << umSplitPoint << endl;
		m_pStack->PushCommand(make_unique<InsertKnotCommand>(idPipe, umSplitPoint));
	}

private:

	inline static const wstring NAME { L"InsertKnot" };

	class Wrapper : public ScriptFunctor
	{
	public:
		void operator() (Script& script) const final
		{
			NobId         const idPipe       { ScrReadNobId(script) };
			MicroMeterPnt const umSplitPoint { ScrReadMicroMeterPnt(script) };
			InsertKnotCommand::Push(idPipe, umSplitPoint);
		}
	};


	unique_ptr<Knot> m_upInsertKnot;
};