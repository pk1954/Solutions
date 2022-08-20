// AttachSigGen2LineCmd.ixx
//
// Commands

module;

#include "NNetModelWriterInterface.h"
#include "NobId.h"
#include "InputLine.h"
#include "SignalGenerator.h"
#include "NNetCommand.h"

export module AttachSigGen2LineCmd;

export class AttachSigGen2LineCmd : public NNetCommand
{
public:
	explicit AttachSigGen2LineCmd(NobId const nobId)
	  : m_inputLine(*m_pNMWI->GetNobPtr<InputLine*>(nobId)),
		m_pSigGenOld(m_inputLine.GetSigGen())
	{}

	void Do() final
	{
		m_inputLine.SetSigGen(m_pSigGenNew);
	}

	void Undo() final
	{
		m_inputLine.SetSigGen(m_pSigGenOld);
	}

private:
	InputLine       & m_inputLine;
	SignalGenerator * m_pSigGenOld;
	SignalGenerator * m_pSigGenNew{ m_pNMWI->GetSigGenSelected() };
};