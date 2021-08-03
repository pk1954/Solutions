// SelectNobCommand.h
//
// Commands

#pragma once

#include "BoolOp.h"
#include "NNetModelWriterInterface.h"
#include "NobId.h"
#include "SelectionCommand.h"
#include "BaseKnot.h"

class SelectNobCommand : public SelectionCommand
{
public:
	SelectNobCommand
	(
		NobId   const id, 
		tBoolOp const op 
	)
	  :	m_idNob(id),
		m_op(op)
	{
		assert(IsDefined(m_idNob));
	}

	virtual void Do(NNetModelWriterInterface & nmwi)
	{ 
		SelectionCommand::Do(nmwi);
		Nob * pNob { nmwi.GetNobPtr<Nob *>(m_idNob) };
		pNob->Select(ApplyOp2(pNob->IsSelected(), m_op));
	}

private:
	NobId   const m_idNob;
	tBoolOp const m_op;
};
