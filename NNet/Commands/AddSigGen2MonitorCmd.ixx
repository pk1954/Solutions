// AddSigGen2MonitorCmd.ixx
//
// Commands

module;

#include <iostream>

export module AddSigGen2MonitorCmd;

import NNetCommand;
import NNetModel;

export class AddSigGen2MonitorCmd : public NNetCommand
{
public:
	explicit AddSigGen2MonitorCmd(TrackNr const trackNr)
		: m_trackNr(trackNr)
	{
		m_pSigGen = m_pNMWI->GetSigGenSelected();
	}

	void Do() final
	{
		m_pNMWI->GetMonitorData().InsertTrack(m_trackNr);
		m_signalId = SignalFactory::MakeSigGenSignal(m_trackNr, *m_pNMWI);
	}

	void Undo() final
	{
		m_pNMWI->GetMonitorData().DeleteSignal(m_signalId);
		m_pNMWI->GetMonitorData().DeleteTrack(m_trackNr);
	};

	static void Register()
	{
		SymbolTable::ScrDefConst(NAME, new Wrapper);
	}

	static void Push(TrackNr const trackNr)
	{
		if (IsTraceOn())
			TraceStream() << NAME << trackNr.GetValue() << endl;
		m_pStack->PushCommand(make_unique<AddSigGen2MonitorCmd>(trackNr));
	}

private:

	inline static const wstring NAME { L"AddSigGen2Monitor" };

	class Wrapper : public ScriptFunctor
	{
	public:
		void operator() (Script& script) const final
		{
			AddSigGen2MonitorCmd(ScrReadTrackNr(script));
		}
	};

	SignalGenerator const * m_pSigGen;
	TrackNr         const   m_trackNr;
	SignalId                m_signalId{};
};