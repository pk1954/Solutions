// NewSigGenCmd.ixx
//
// Commands

module;

#include <memory>
#include "Resource.h"

export module NewSigGenCmd;

import Command;
import SigGenCommand;
import NNetModel;

export class NewSigGenCmd : public SigGenCommand
{
public:
	NewSigGenCmd()
	{
		m_upSigGen = m_pNMWI->NewSigGen();
	}

	void Do() final 
	{ 
		m_sigGenIdNew = m_pNMWI->PushSigGen(move(m_upSigGen));
		PostCommand2Application(IDD_REGISTER_SIG_GEN, m_sigGenIdNew.GetValue());
		SetActiveSigGenId(m_sigGenIdNew);
	}

	void Undo() final 
	{ 
		m_upSigGen = m_pNMWI->PopSigGen();
		SetActiveSigGenId(m_sigGenIdOld);
	}

private:
	UPSigGen m_upSigGen;
	SigGenId m_sigGenIdNew;
	SigGenId m_sigGenIdOld;
};