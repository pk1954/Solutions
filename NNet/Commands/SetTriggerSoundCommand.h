// SetTriggerSoundCommand.h
//
// Commands

#pragma once

#include "NNetModelWriterInterface.h"
#include "NobId.h"
#include "NNetCommand.h"
#include "Neuron.h"

import SoundInterface;

class SetTriggerSoundCommand : public NNetCommand
{
public:
	SetTriggerSoundCommand
	(
		NobId      const   id, 
		SoundDescr const & sound
	)
	  : m_idNeuron(id),
		m_sound(sound)
	{ }

	void Do() final
	{ 
		Nob * const pNob { m_pNMWI->GetNob(m_idNeuron) };
		assert( pNob->IsAnyNeuron() );
		static_cast<Neuron *>(pNob)->SetTriggerSound(m_sound);
		m_pNMWI->ClearDynamicData();
	}

	void Undo() final
	{ 
		Do();
	}

private:
	NobId const m_idNeuron;
	SoundDescr  m_sound;
};
