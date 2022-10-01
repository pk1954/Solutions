// ToggleStopOnTriggerCmd.ixx
//
// Commands

export module ToggleStopOnTriggerCmd;

import NNetCommand;
import NNetModel;

export class ToggleStopOnTriggerCmd : public NNetCommand
{
public:
	explicit ToggleStopOnTriggerCmd(NobId const id)
		:	m_id(id)
	{ }

	void Do() final
	{
		m_pNMWI->ToggleStopOnTrigger(m_id);
	}

	void Undo() final
	{
		m_pNMWI->ToggleStopOnTrigger(m_id);
	}

private:
	NobId const m_id;
};

