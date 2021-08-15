// RotateModelCommand.h
//
// Commands

#pragma once

#include "MoreTypes.h"
#include "NNetModelReaderInterface.h"
#include "NNetModelWriterInterface.h"
#include "RotationCommand.h"

class RotateModelCommand : public RotationCommand
{
public:
	RotateModelCommand
	(
		NNetModelReaderInterface & nmri,
		MicroMeterPnt      const & umPntOld, 
		MicroMeterPnt      const & umPntNew
	)
	{
		m_umPntPivot = nmri.GetUPNobs().CenterOfGravity
		(
			[&](Nob const & nob){ return nob.IsAnyNeuron(); }
		);
		calcRadDelta(umPntOld, umPntNew);
	}

	virtual void Do(NNetModelWriterInterface & nmwi) 
	{ 
		nmwi.GetUPNobs().Apply2All<BaseKnot>([&](BaseKnot & b) { b.RotateNob(m_umPntPivot, m_radDelta); });
	}

	virtual void Undo(NNetModelWriterInterface & nmwi) 
	{ 
		nmwi.GetUPNobs().Apply2All<BaseKnot>([&](BaseKnot & b) { b.RotateNob(m_umPntPivot, -m_radDelta); });
	}

	virtual NobId const GetAffectedNob() const { return NO_NOB;	}
};
