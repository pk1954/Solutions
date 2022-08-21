// AttachSigGen2SelCmd.ixx
//
// Commands

module;

#include <vector>
#include "Nob.h"
#include "NobId.h"
#include "InputLine.h"
#include "SignalGenerator.h"
#include "NNetCommand.h"

export module AttachSigGen2SelCmd;

using std::vector;

export class AttachSigGen2SelCmd : public NNetCommand
{
public:
	AttachSigGen2SelCmd()
	{
		m_pNMWI->GetUPNobs().Apply2AllSelected<InputLine>
		(
			[this](InputLine & inputLine) { push(&inputLine); }
		);
	}

	void Do() final
	{
		m_pNMWI->GetUPNobs().Apply2AllSelected<InputLine>
		(
			[this](InputLine & inputLine) { inputLine.SetSigGen(m_pSigGenNew); }
		);
	}

	void Undo() final
	{
		for (auto i : m_list)
		{
			m_pNMWI->GetNobPtr<InputLine*>(i.id)->SetSigGen(i.pSigGenOld);
		}
	}

private:
	struct SigGenAttachment
	{
		NobId             id;
		SignalGenerator * pSigGenOld;
	};
	vector<SigGenAttachment> m_list;
	SignalGenerator        * m_pSigGenNew{ m_pNMWI->GetSigGenSelected() };

	void push(InputLine * pInputLine)
	{
		m_list.push_back(SigGenAttachment(pInputLine->GetId(), pInputLine->GetSigGen()));
	}
};