// SigGenDynamicData.h
//
// NNetModel

#pragma once

#include "BasicTypes.h"

import MoreTypes;
import VoltageType;

class SigGenStaticData;
class Param;

class SigGenDynamicData
{
public:

	void Reset();
	void StartSpike();
	void StartStimulus();
	void StopStimulus();

	fMicroSecs GetStimulusTime() const;

	mV SetTime
	(
		SigGenStaticData const &,
		Param            const &
	);

	mV SetTime
	(
		SigGenStaticData const &,
		Param            const &,
		fMicroSecs       const
	);

	bool IsStimulusActive() const { return m_stimulusActive; }

private:
	bool       m_stimulusActive   { false };
	fMicroSecs m_usSimuStartStimu { 0.0_MicroSecs }; // in simu time
	fMicroSecs m_usSimuStartSpike { 0.0_MicroSecs }; // in simu time
};