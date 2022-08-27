// AttachSigGen2ConCmd.ixx
//
// Commands

module;

#include "NNetModelWriterInterface.h"
#include "SignalGenerator.h"
#include "InputConnector.h"

export module AttachSigGen2ConCmd;

import NobId;
import NNetCommand;

export class AttachSigGen2ConCmd : public NNetCommand
{
public:
	explicit AttachSigGen2ConCmd(NobId const nobId)
	  : m_inputConnector(*m_pNMWI->GetNobPtr<InputConnector*>(nobId)),
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

private:
	InputConnector  & m_inputConnector;
	SignalGenerator * m_pSigGenOld;
	SignalGenerator * m_pSigGenNew { m_pNMWI->GetSigGenSelected() };
};