// UtilityWrappers.h
//
// Utilities

#pragma once

#include "SaveCast.h"
#include "Script.h"
#include "PixelTypes.h"

import BoolOp;

class Script;

tBoolOp       ScrReadBoolOp       (Script &);
PIXEL         ScrReadPixel        (Script &);
PixelPoint    ScrReadPixelPoint   (Script &);
PixelRectSize ScrReadPixelRectSize(Script &);
PixelRect     ScrReadPixelRect    (Script &);
fPixel        ScrReadfPixel       (Script &);
fPixelPoint   ScrReadfPixelPoint  (Script &);

template <typename T>
T ScrRead(Script& script)
{
    return static_cast<T>(Cast2Float(script.ScrReadFloat()));
}

template<typename T>
BASE_PEAK<T> ScrReadBasePeak(Script & script)
{
    T const base { ScrRead<T>(script) };
    T const peak { ScrRead<T>(script) };
    return BASE_PEAK<T>(base, peak);
}

void DefineUtilityWrapperFunctions();