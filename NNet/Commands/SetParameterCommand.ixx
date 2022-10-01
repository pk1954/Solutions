// SetParameterCommand.ixx
//
// Commands

export module SetParameterCommand;

import NNetCommand;
import NNetModel;

export class SetParameterCommand : public NNetCommand
{
public:
	SetParameterCommand
	(
		NNetParameters   const & param,
		ParamType::Value const   parameter,
		float            const   fNewValue
	)
	  : m_parameter(parameter),
		m_fOldValue(param.GetParameterValue(parameter)),
		m_fNewValue(fNewValue)
	{ }

	void Do() final
	{
		m_pNMWI->SetParam(m_parameter, m_fNewValue);
	}

	void Undo() final
	{
		m_pNMWI->SetParam(m_parameter, m_fOldValue);
	}

private:
	ParamType::Value const m_parameter;
	float            const m_fOldValue;
	float            const m_fNewValue;
};
