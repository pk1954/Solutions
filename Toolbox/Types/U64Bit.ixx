// U64Bit.ixx
//
// Toolbox\Types

module;

#include <cstdint>

export module Types:U64Bit;

import :MoreTypes;
import :BasicTypes;

export union U64Bit
{
    struct twoFloats
    {
        float floatA;
        float floatB;
    } f2;
    struct twoLongs
    {
        long longA;
        long longB;
    } l2;
    struct twoInts
    {
        int intA;
        int intB;
    } i2;
    struct twoULongs
    {
        unsigned long ulA;
        unsigned long ulB;
    } ul2;
    uint64_t ui64;
};

export inline uint64_t Pack2UINT64(MicroMeterPnt const pnt)
{
    U64Bit u;
    u.f2.floatA = pnt.GetXvalue();
    u.f2.floatB = pnt.GetYvalue();
    return u.ui64;
}

export inline MicroMeterPnt Unpack2MicroMeterPnt(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return MicroMeterPnt(MicroMeter(u.f2.floatA), MicroMeter(u.f2.floatB));
}

export inline uint64_t Pack2UINT64(int const iA, int const iB)
{
    U64Bit u;
    u.i2.intA = iA;
    u.i2.intB = iB;
    return u.ui64;
}

export inline int UnpackIntA(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return u.i2.intA;
}

export inline int UnpackIntB(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return u.i2.intB;
}

export inline uint64_t Pack2UINT64(long const lA, long const lB)
{
    U64Bit u;
    u.l2.longA = lA;
    u.l2.longB = lB;
    return u.ui64;
}

export inline long UnpackLongA(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return u.l2.longA;
}

export inline long UnpackLongB(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return u.l2.longB;
}

export inline uint64_t Pack2UINT64(unsigned long const ulA, unsigned long const ulB)
{
    U64Bit u;
    u.ul2.ulA = ulA;
    u.ul2.ulB = ulB;
    return u.ui64;
}

export inline unsigned long UnpackUlongA(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return u.ul2.ulA;
}

export inline unsigned long UnpackUlongB(uint64_t ui64)
{
    U64Bit u;
    u.ui64 = ui64;
    return u.ul2.ulB;
}
