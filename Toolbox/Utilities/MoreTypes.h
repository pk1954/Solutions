// MoreTypes.h
//
// Utilities

#pragma once

#include "NamedType.h"
#include "BasicTypes.h"
#include "PointType.h"
#include "RectType.h"
#include "LineType.h"
#include "CircleType.h"
#include "EllipseType.h"

using MicroMeterPnt = PosType<MicroMeter>;

inline static const MicroMeterPnt NP_NULL(MicroMeterPnt::NULL_VAL());   // compiler generates call!
inline static const MicroMeterPnt NP_ZERO(MicroMeterPnt::ZERO_VAL());   // compiler generates call!

static MicroMeterPnt Radian2Vector(Radian const r)
{
	return MicroMeterPnt(Cos(r), Sin(r));
}

static Radian Vector2Radian(MicroMeterPnt const & umPnt)
{
	return Radian(atan2(umPnt.GetYvalue(),umPnt.GetXvalue()));
}

using MicroMeterRect     = RectType<MicroMeter>;
using MicroMeterRectSize = SizeType<MicroMeter>;
using MicroMeterLine     = LineType<MicroMeter>;
using MicroMeterCircle   = CircleType <MicroMeter>;
using MicroMeterEllipse  = EllipseType<MicroMeter>;

template <typename UNIT>
struct BASE_PEAK
{
	UNIT base;
	UNIT peak;

	bool operator==(BASE_PEAK const & rhs) const = default;
};
