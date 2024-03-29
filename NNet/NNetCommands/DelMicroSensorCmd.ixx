// DelMicroSensorCmd.ixx
//
// NNetCommands

module;

#include <iostream>

export module NNetCommands:DelMicroSensorCmd;

import IoUtil;
import Signals;
import :NNetCommand;

export class DelMicroSensorCmd : public NNetCommand
{
public:

	DelMicroSensorCmd(NobId const nobId)
		: m_pNob(m_pNMWI->GetNob(nobId))
	{
		m_signalId = m_pNMWI->FindSignalId
		(
			[this](Signal const& s)
			{
				return static_cast<NNetSignal const &>(s).GetSignalSource() == m_pNob->GetMicroSensor(); 
			}
		);
	}

	void Do() final
	{
		m_upMicroSensor = m_pNob->DeleteMicroSensor();
		m_upSignal      = move(m_pNMWI->GetMonitorData().DeleteSignal(m_signalId));
		PlaySound(L"DISAPPEAR_SOUND");
	};

	void Undo() final
	{
		m_pNob->SetMicroSensor(move(m_upMicroSensor));
		m_pNMWI->GetMonitorData().AddSignal(m_signalId, move(m_upSignal));
		PlaySound(L"SNAP_IN_SOUND");
	};

	static void Register()
	{
		SymbolTable::ScrDefConst(NAME, &m_wrapper);
	}

	static void Push(NobId const nobId)
	{
		if (IsTraceOn())
			TraceStream() << NAME << nobId << endl;
		PushCommand(make_unique<DelMicroSensorCmd>(nobId));
	}

private:

	inline static const wstring NAME { L"DelMicroSensor" };

	inline static struct myWrapper : public Wrapper
	{
		using Wrapper::Wrapper;
		void operator() (Script& script) const final
		{
			DelMicroSensorCmd::Push(ScrReadNobId(script));
		}
	} m_wrapper { NAME };

	Nob                   * m_pNob          { nullptr };
	SignalId                m_signalId      {};
	unique_ptr<Signal>      m_upSignal      {};
	unique_ptr<MicroSensor> m_upMicroSensor {};
};
