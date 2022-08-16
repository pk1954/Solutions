// SpeedType.h
//
// Utilities

module;

#include "BasicTypes.h"

export module SpeedType;

import NamedType;
import SaveCast;

export using meterPerSec = NamedType<float, struct meterPerSec_Parameter >;

export constexpr meterPerSec operator"" _meterPerSec(const long double d)
{ 
	return meterPerSec(Cast2Float(d));
}

export MicroMeter CoveredDistance(meterPerSec const speed, fMicroSecs const time)
{
	return MicroMeter(speed.GetValue() * time.GetValue());
}