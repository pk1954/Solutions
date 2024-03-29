// ParameterType.cpp
//
// NNetModel

module;

#include <iostream>
#include <unordered_map>

module ParamType;

import Types;

using std::unordered_map;
using std::wostream;

wchar_t const * ParamType::GetName(ParamType::Value const p)
{
	using enum ParamType::Value;
	static unordered_map < ParamType::Value, wchar_t const * const > mapParam =
	{
		{ inputPeakTime,  L"StimulusMaxTime"  }, // stimulus time til peak
		{ inputPeakFreq,  L"StimulusMaxFreq"  }, // stimulus maximum frequency
		{ inputPeakVolt,  L"StimulusMaxVolt"  }, // max amplitude of input connector stimuli
		{ inputBaseFreq,  L"inputBaseFreq"    }, // frequency of normal input connector pulses
		{ inputBaseVolt,  L"inputBaseVolt"    }, // amplitude of normal input connector pulses
		{ neuronPeakVolt, L"PeakVoltage"      }, // peak voltage when neuron triggers
		{ pulseSpeed,     L"PulseSpeed"       },
		{ spikeWidth,     L"SpikeWidth"       },
		{ threshold,      L"Threshold"        },
    	{ refractPeriod,  L"RefractoryPeriod" },
	    { timeResolution, L"TimeResolution"   },
		{ filterSize,     L"FilterSize"       }, // 
		{ baseFrequency,  L"BaseFrequency"    },   // Legacy
		{ signalLoss,     L"SignalLoss"       },   // Legacy
		{ pulseWidth,     L"PulseWidth"       },   // Legacy
		{ stdPulseRate,   L"StdPulseRate"     },   // Legacy
		{ pulseRate,      L"PulseRate"        }    // Legacy
	};				  
	
	return mapParam.at(p);
}

wchar_t const * ParamType::GetUnit(ParamType::Value const p)
{
	using enum ParamType::Value;
	static unordered_map < ParamType::Value, wchar_t const * const > mapParam =
	{
		{ inputPeakFreq,   L"Hz"    },
		{ inputPeakTime,   L"�s"    },
		{ inputPeakVolt,   L"mV"    },
		{ inputBaseVolt,   L"mV"    },
		{ baseFrequency,   L"Hz"    },
		{ pulseSpeed,      L"m/sec" },
		{ spikeWidth,      L"�s"    },
		{ threshold,       L"mV"    },
		{ neuronPeakVolt,  L"mV"    },
    	{ refractPeriod,   L"�s"    },
	    { timeResolution,  L"�s"    },
		{ filterSize,      L"�s"    },
		{ signalLoss,      L"1/�m"  },  // Legacy
		{ stdPulseRate,    L"Hz"    },  // Legacy
		{ pulseWidth,      L"�s"    },  // Legacy
		{ pulseRate,       L"Hz"    }   // Legacy
	};				  

	return mapParam.at(p);
}

wostream & operator <<(wostream & out, ParamType::Value const p)
{
	out << L" " << ParamType::GetName(p);
	return out;
}
